
#include <node.h>
#include "sophist.h"
#include "database.h"
#include "database_workers.h"
#include "iterator.h"

using namespace sophia;

namespace sophist {

static v8::Persistent<v8::FunctionTemplate> database_constructor;

Database::Database(char *path) : path(path) {}

Database::~Database() {
  delete sophia;
  delete path;
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

  exports->Set(NanNew("Database"), tpl->GetFunction());
}

NAN_METHOD(Database::New) {
  NanScope();

  if (0 == args.Length() || !args[0]->IsString()) {
    NanThrowError("path required");
    NanReturnUndefined();
  }

  char *path = StringToCharArray(args[0]->ToString());
  Database *self = new Database(path);
  self->sophia = new Sophia(path);
  self->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(Database::OpenSync) {
  NanScope();
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());

  SophiaReturnCode rc = self->sophia->Open();
  if (SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sophia->Error(rc));
  }
  NanReturnUndefined();
}

NAN_METHOD(Database::Open) {
  NanScope();

  if (0 == args.Length() || !args[0]->IsFunction()) {
    NanThrowError("callback required");
    NanReturnUndefined();
  }

  v8::Local<v8::Function> callback = args[0].As<v8::Function>();
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());

  OpenWorker *worker = new OpenWorker(
      self
    , new NanCallback(callback)
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("database", _this);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Database::CloseSync) {
  NanScope();
  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());

  SophiaReturnCode rc = self->sophia->Close();
  if (SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sophia->Error(rc));
  }
  NanReturnUndefined();
}

NAN_METHOD(Database::Close) {
  NanScope();

  if (0 == args.Length() || !args[0]->IsFunction()) {
    NanThrowError("callback required");
    NanReturnUndefined();
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
    NanThrowError("key/value required");
    NanReturnUndefined();
  }

  char *key = StringToCharArray(args[0]->ToString());
  char *value = StringToCharArray(args[1]->ToString());

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

  if (3 > args.Length()) {
    NanThrowError("insufficient arguments");
    NanReturnUndefined();
  }

  if (!args[0]->IsString() || !args[1]->IsString()) {
    NanThrowError("key/value required");
    NanReturnUndefined();
  }

  if (!args[2]->IsFunction()) {
    NanThrowError("callback required");
    NanReturnUndefined();
  }

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  char *key = StringToCharArray(args[0]->ToString());
  char *value = StringToCharArray(args[1]->ToString());
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
    NanThrowError("key required");
    NanReturnUndefined();
  }

  char *key = StringToCharArray(args[0]->ToString());
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
    NanThrowError("key/callback required");
    NanReturnUndefined();
  }

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  char *key = StringToCharArray(args[0]->ToString());
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
    NanThrowError("key required");
    NanReturnUndefined();
  }

  char *key = StringToCharArray(args[0]->ToString());

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
    NanThrowError("key/callback required");
    NanReturnUndefined();
  }

  Database *self = node::ObjectWrap::Unwrap<Database>(args.This());
  char *key = StringToCharArray(args[0]->ToString());
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
  v8::Local<v8::Object> it = Iterator::NewInstance(args.This());
  NanReturnValue(it);
}

}; // namespace sophist
