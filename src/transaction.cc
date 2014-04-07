
#include <node.h>
#include <nan.h>
#include "sophist.h"
#include "transaction.h"

namespace sophist {

v8::Persistent<v8::FunctionTemplate> Transaction::constructor;

/**
 * Async commit worker.
 */

class CommitWorker : public NanAsyncWorker {
public:
  CommitWorker(Transaction *self, NanCallback *callback)
  : NanAsyncWorker(callback), self(self) {}

  void
  Execute() {
    sophia::SophiaReturnCode rc = self->t->Commit();
    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

private:
  Transaction *self;
};

/**
 * Async rollback worker.
 */

class RollbackWorker : public NanAsyncWorker {
public:
  RollbackWorker(Transaction *self, NanCallback *callback)
  : NanAsyncWorker(callback), self(self) {}

  void
  Execute() {
    sophia::SophiaReturnCode rc = self->t->Rollback();
    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

private:
  Transaction *self;
};

Transaction::Transaction() {}

Transaction::~Transaction() {}

void
Transaction::Init() {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(
    Transaction::New
  );
  NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);
  tpl->SetClassName(NanSymbol("Transaction"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "commit", Commit);
  NODE_SET_PROTOTYPE_METHOD(tpl, "rollback", Rollback);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set", Set);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);
}

v8::Local<v8::Object>
Transaction::NewInstance(v8::Local<v8::Object> sophist) {
  NanScope();
  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> c = NanPersistentToLocal(
    constructor
  );
  v8::Handle<v8::Value> argv[1] = { sophist };
  instance = c->GetFunction()->NewInstance(1, argv);
  return instance;
}

NAN_METHOD(Transaction::New) {
  NanScope();
  Sophist *sophist = ObjectWrap::Unwrap<Sophist>(args[0]->ToObject());
  Transaction *self = new Transaction;

  if (!sophist->sp->IsOpen()) {
    return NanThrowError(
      "Unable to create a transaction on an unopen database"
    );
  }

  self->Wrap(args.This());
  self->sophist = sophist;
  self->sp = sophist->sp;
  self->t = new sophia::Transaction(self->sp);

  sophia::SophiaReturnCode rc = self->t->Begin();
  if (sophia::SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sp->Error(rc));
  }

  NanReturnValue(args.This());
}

NAN_METHOD(Transaction::Commit) {
  NanScope();
  Transaction *self = node::ObjectWrap::Unwrap<Transaction>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  CommitWorker *worker = new CommitWorker(self, new NanCallback(fn));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Transaction::Rollback) {
  NanScope();
  Transaction *self = node::ObjectWrap::Unwrap<Transaction>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  RollbackWorker *worker = new RollbackWorker(self, new NanCallback(fn));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Transaction::Set) {
  NanScope();
  size_t keysize;
  size_t valuesize;
  char *key = NanCString(args[0], &keysize);
  char *value = NanCString(args[1], &valuesize);
  Transaction *self = node::ObjectWrap::Unwrap<Transaction>(args.This());

  sophia::SophiaReturnCode rc = self->t->Set(
      key
    // , keysize
    , value
    // , valuesize
  );

  delete key;
  delete value;

  if (sophia::SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sp->Error(rc));
  }
  NanReturnUndefined();
}

NAN_METHOD(Transaction::Delete) {
  NanScope();
  size_t keysize;
  char *key = NanCString(args[0], &keysize);
  Transaction *self = node::ObjectWrap::Unwrap<Transaction>(args.This());
  sophia::SophiaReturnCode rc = self->t->Delete(key);
  delete key;
  if (sophia::SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sp->Error(rc));
  }
  NanReturnUndefined();
}

} // namespace sophist
