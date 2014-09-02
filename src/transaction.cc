
#include "transaction.h"
#include "transaction_workers.h"
#include "sophist.h"

namespace sophist {

static v8::Persistent<v8::FunctionTemplate> transaction_constructor;

Transaction::Transaction(Database *database) : database(database) {}

Transaction::~Transaction() {
  delete t;
}

void Transaction::Init() {
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(
    Transaction::New
  );
  NanAssignPersistent(transaction_constructor, tpl);
  tpl->SetClassName(NanNew("Transaction"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set", Transaction::Set);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Transaction::Delete);
  NODE_SET_PROTOTYPE_METHOD(tpl, "commit", Transaction::Commit);
  NODE_SET_PROTOTYPE_METHOD(tpl, "rollback", Transaction::Rollback);
}

v8::Local<v8::Object> Transaction::NewInstance(
  v8::Local<v8::Object> database
) {
  NanEscapableScope();

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> c = NanNew<v8::FunctionTemplate>(
    transaction_constructor
  );

  // TODO: options, etc
  v8::Handle<v8::Value> argv[1] = { database };
  instance = c->GetFunction()->NewInstance(1, argv);

  return NanEscapeScope(instance);
}

NAN_METHOD(Transaction::New) {
  NanScope();
  Database *database = node::ObjectWrap::Unwrap<Database>(
    args[0]->ToObject()
  );
  Transaction *transaction = new Transaction(database);
  transaction->t = new sophia::Transaction(database->sophia);

  sophia::SophiaReturnCode rc = transaction->t->Begin();
  if (sophia::SOPHIA_SUCCESS != rc) {
    return NanThrowError(database->sophia->Error(rc));
  }

  transaction->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(Transaction::Set) {
  NanScope();

  if (2 > args.Length() || !args[0]->IsString() || !args[1]->IsString()) {
    return NanThrowError("key/value required");
  }

  char *key, *value;
  SP_V8_STRING_TO_CHAR_ARRAY(key, args[0]);
  SP_V8_STRING_TO_CHAR_ARRAY(value, args[1]);
  Transaction *transaction = node::ObjectWrap::Unwrap<Transaction>(
    args.This()
  );
  sophia::SophiaReturnCode rc = transaction->t->Set(key, value);
  delete[] key;
  delete[] value;
  if (sophia::SOPHIA_SUCCESS != rc) {
    return NanThrowError(
      transaction->database->sophia->Error(rc)
    );
  }

  NanReturnUndefined();
}

NAN_METHOD(Transaction::Delete) {
  NanScope();
  if (0 ==  args.Length() || !args[0]->IsString()) {
    return NanThrowError("key required");
  }

  char *key;
  SP_V8_STRING_TO_CHAR_ARRAY(key, args[0]);
  Transaction *transaction = node::ObjectWrap::Unwrap<Transaction>(
    args.This()
  );
  sophia::SophiaReturnCode rc = transaction->t->Delete(key);
  delete[] key;
  if (sophia::SOPHIA_SUCCESS != rc) {
    return NanThrowError(
      transaction->database->sophia->Error(rc)
    );
  }
  NanReturnUndefined();
}

NAN_METHOD(Transaction::Commit) {
  NanScope();
  if (0 == args.Length() || !args[0]->IsFunction()) {
    return NanThrowError("callback required");
  }
  v8::Local<v8::Function> callback = args[0].As<v8::Function>();
  Transaction *transaction = node::ObjectWrap::Unwrap<Transaction>(
    args.This()
  );
  CommitWorker *worker = new CommitWorker(
      transaction
    , new NanCallback(callback)
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("transaction", _this);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Transaction::Rollback) {
  NanScope();
  if (0 == args.Length() || !args[0]->IsFunction()) {
    return NanThrowError("callback required");
  }
  v8::Local<v8::Function> callback = args[0].As<v8::Function>();
  Transaction *transaction = node::ObjectWrap::Unwrap<Transaction>(
    args.This()
  );
  RollbackWorker *worker = new RollbackWorker(
      transaction
    , new NanCallback(callback)
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("transaction", _this);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

} // namespace sophist
