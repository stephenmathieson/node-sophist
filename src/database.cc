
#include <node.h>
#include "sophist.h"
#include "database.h"
#include "database_workers.h"
#include "iterator.h"

using namespace sophia;

namespace sophist {

static v8::Persistent<v8::FunctionTemplate> database_constructor;

Database::Database(char *path) : path(path) {
  currentIteratorId = 0;
  transaction = NULL;
}

Database::~Database() {
  delete sophia;
  delete[] path;
}

void Database::Init(v8::Handle<v8::Object> exports) {
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(
    Database::New
  );
  NanAssignPersistent(database_constructor, tpl);

  tpl->SetClassName(NanNew("Database"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Database::Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "openSync", Database::OpenSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Database::Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "closeSync", Database::CloseSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set", Database::Set);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setSync", Database::SetSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSync", Database::GetSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Database::Get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "deleteSync", Database::DeleteSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Database::Delete);
  NODE_SET_PROTOTYPE_METHOD(tpl, "iterator", Database::Iterator);
  NODE_SET_PROTOTYPE_METHOD(tpl, "transaction", Database::Transaction);

  exports->Set(NanNew("Database"), tpl->GetFunction());
}

void Database::ReleaseIterator(uint32_t id) {
  iterators.erase(id);
}

void Database::ReleaseIterators() {
  // cleanup any open iterators
  if (!iterators.empty()) {
    std::map<uint32_t, sophist::Iterator *>::iterator it;
    // loop, ending/releasing each open iterator
    for (it = iterators.begin(); it != iterators.end() ; ++it) {
      uint32_t id = it->first;
      sophist::Iterator *iterator = it->second;
      iterator->it->End();
      ReleaseIterator(id);
    }
  }
}

NAN_METHOD(Database::New) {
  NanScope();

  if (0 == args.Length() || !args[0]->IsString()) {
    NanThrowError("path required");
    NanReturnUndefined();
  }

  char *path;
  SP_V8_STRING_TO_CHAR_ARRAY(path, args[0]);
  Database *self = new Database(path);
  self->sophia = new Sophia(path);
  self->Wrap(args.This());
  NanReturnValue(args.This());
}

#define SP_PARSE_OPEN_OPTIONS()                        \
  bool create_if_missing = NanBooleanOptionValue(      \
      options                                          \
    , NanNew("createIfMissing")                        \
    , true                                             \
  );                                                   \
  bool read_only = NanBooleanOptionValue(              \
      options                                          \
    , NanNew("readOnly")                               \
    , false                                            \
  );                                                   \
  uint32_t merge_watermark = NanUInt32OptionValue(     \
      options                                          \
    , NanNew("mergeWatermark")                         \
    , 100000                                           \
  );                                                   \
  uint32_t page_size = NanUInt32OptionValue(           \
      options                                          \
    , NanNew("pageSize")                               \
    , 2048                                             \
  );

NAN_METHOD(Database::OpenSync) {
  NanScope();
  v8::Local<v8::Object> options;
  if (args.Length() && args[0]->IsObject()) {
    options = args[0].As<v8::Object>();
  }
  SP_PARSE_OPEN_OPTIONS()
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  SophiaReturnCode rc = self->sophia->Open(
      create_if_missing
    , read_only
    , page_size
    , merge_watermark
  );
  if (SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sophia->Error(rc));
  }
  NanReturnUndefined();
}

NAN_METHOD(Database::Open) {
  NanScope();
  v8::Local<v8::Object> options;
  v8::Local<v8::Function> callback;

  if (0 == args.Length()) return NanThrowError("callback required");
  if (args[0]->IsFunction()) {
    callback = args[0].As<v8::Function>();
  } else if (args[1]->IsFunction()) {
    options = args[0].As<v8::Object>();
    callback = args[1].As<v8::Function>();
  } else {
    return NanThrowError("callback required");
  }

  SP_PARSE_OPEN_OPTIONS()
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  OpenWorker *worker = new OpenWorker(
      self
    , new NanCallback(callback)
    , create_if_missing
    , read_only
    , merge_watermark
    , page_size
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("database", _this);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

#undef SP_PARSE_OPEN_OPTIONS

NAN_METHOD(Database::CloseSync) {
  NanScope();
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  // cleanup iterators (if any)
  self->ReleaseIterators();
  SophiaReturnCode rc = self->sophia->Close();
  if (SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sophia->Error(rc));
  }
  NanReturnUndefined();
}

NAN_METHOD(Database::Close) {
  NanScope();

  if (0 == args.Length() || !args[0]->IsFunction()) {
   return NanThrowError("callback required");
  }

  v8::Local<v8::Function> callback = args[0].As<v8::Function>();
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());

  CloseWorker *worker = new CloseWorker(
      self
    , new NanCallback(callback)
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("database", _this);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Database::SetSync) {
  NanScope();
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());

  if (2 > args.Length() || !args[0]->IsString() || !args[1]->IsString()) {
    return NanThrowError("key/value required");
  }

  char *key, *value;
  SP_V8_STRING_TO_CHAR_ARRAY(key, args[0]);
  SP_V8_STRING_TO_CHAR_ARRAY(value, args[1]);

  SophiaReturnCode rc = self->sophia->Set(key, value);

  delete[] key;
  delete[] value;

  if (SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sophia->Error(rc));
  }

  NanReturnUndefined();
}

NAN_METHOD(Database::Set) {
  NanScope();

  if (3 > args.Length()) return NanThrowError("insufficient arguments");
  if (!args[0]->IsString() || !args[1]->IsString()) {
    return NanThrowError("key/value required");
  }
  if (!args[2]->IsFunction()) return NanThrowError("callback required");

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  char *key, *value;
  SP_V8_STRING_TO_CHAR_ARRAY(key, args[0]);
  SP_V8_STRING_TO_CHAR_ARRAY(value, args[1]);
  v8::Local<v8::Function> callback = args[2].As<v8::Function>();

  SetWorker *worker = new SetWorker(
      self
    , new NanCallback(callback)
    , key
    , value
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("database", _this);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Database::GetSync) {
  NanScope();

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());

  if (0 == args.Length() || !args[0]->IsString()) {
    return NanThrowError("key required");
  }

  char *key;
  SP_V8_STRING_TO_CHAR_ARRAY(key, args[0]);

  char *value = self->sophia->Get(key);

  delete[] key;

  if (value) {
    NanReturnValue(NanNew<v8::String>(value));
  } else {
    NanReturnNull();
  }
}

NAN_METHOD(Database::Get) {
  NanScope();

  if (2 > args.Length() || !args[0]->IsString() || !args[1]->IsFunction()) {
    return NanThrowError("key/callback required");
  }

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  char *key;
  SP_V8_STRING_TO_CHAR_ARRAY(key, args[0]);
  v8::Local<v8::Function> callback = args[1].As<v8::Function>();

  GetWorker *worker = new GetWorker(
      self
    , new NanCallback(callback)
    , key
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("database", _this);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Database::DeleteSync) {
  NanScope();

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());

  if (0 == args.Length() || !args[0]->IsString()) {
    return NanThrowError("key required");
  }

  char *key;
  SP_V8_STRING_TO_CHAR_ARRAY(key, args[0]);

  SophiaReturnCode rc = self->sophia->Delete(key);

  delete[] key;

  if (SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sophia->Error(rc));
  }

  NanReturnUndefined();
}

NAN_METHOD(Database::Delete) {
  NanScope();

  if (2 > args.Length() || !args[0]->IsString() || !args[1]->IsFunction()) {
    return NanThrowError("key/callback required");
  }

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  char *key;
  SP_V8_STRING_TO_CHAR_ARRAY(key, args[0]);
  v8::Local<v8::Function> callback = args[1].As<v8::Function>();

  DeleteWorker *worker = new DeleteWorker(
      self
    , new NanCallback(callback)
    , key
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("database", _this);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Database::Iterator) {
  NanScope();
  v8::Local<v8::Object> options;
  if (args.Length() && args[0]->IsObject()) {
    options = args[0].As<v8::Object>();
  }
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  uint32_t id = self->currentIteratorId++;
  v8::Local<v8::Object> iteratorHandle = Iterator::NewInstance(
      args.This()
    , NanNew<v8::Number>(id)
    , options
  );
  sophist::Iterator *iterator = node::ObjectWrap::Unwrap<sophist::Iterator>(
    iteratorHandle
  );
  self->iterators[id] = iterator;
  NanReturnValue(iteratorHandle);
}

NAN_METHOD(Database::Transaction) {
  NanScope();
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  if (NULL != self->transaction) {
    return NanThrowError("another transaction is already open");
  }
  v8::Local<v8::Object> transactionHandle = Transaction::NewInstance(
    args.This()
  );
  sophist::Transaction *transaction =
    node::ObjectWrap::Unwrap<sophist::Transaction>(
      transactionHandle
    );
  self->transaction = transaction;
  NanReturnValue(transactionHandle);
}

}; // namespace sophist
