
#include <node.h>
#include <nan.h>
#include "sophia-cc.h"
#include "sophist.h"
#include "iterator.h"
#include "transaction.h"

namespace sophist {

v8::Persistent<v8::FunctionTemplate> Sophist::constructor;

/**
 * Async open worker.
 */

class OpenWorker : public NanAsyncWorker {
public:
  OpenWorker(
      Sophist *self
    , bool create_if_missing
    , bool read_only
    , uint32_t merge_watermark
    , uint32_t page_size
    , NanCallback *callback
  ) : NanAsyncWorker(callback)
    , self(self)
    , create_if_missing(create_if_missing)
    , read_only(read_only)
    , merge_watermark(merge_watermark)
    , page_size(page_size) {}

  void
  Execute() {
    // TODO: maybe fail?
    if (self->sp->IsOpen()) return;

    sophia::SophiaReturnCode rc = self->sp->Open(
        create_if_missing
      , read_only
      , page_size
      , merge_watermark
    );

    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

private:
  Sophist *self;
  bool create_if_missing;
  bool read_only;
  uint32_t merge_watermark;
  uint32_t page_size;
};

/**
 * Async close worker.
 */

class CloseWorker : public NanAsyncWorker {
public:
  CloseWorker(
      Sophist *self
    , NanCallback *callback
  ) : NanAsyncWorker(callback), self(self) {}

  void
  Execute() {
    if (!self->sp->IsOpen()) {
      errmsg = strdup("Unable to close an unopen database");
      return;
    }

    sophia::SophiaReturnCode rc = self->sp->Close();
    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

private:
  Sophist *self;
};

/**
 * Async get worker.
 */

class GetWorker : public NanAsyncWorker {
public:
  GetWorker(
    Sophist *self
    , char *key
    , NanCallback *callback
  ) : NanAsyncWorker(callback)
    , self(self)
    , key(key) {
    value = NULL;
  }

  void
  Execute() {
    value = self->sp->Get(key);
  }

  void
  HandleOKCallback() {
    v8::Local<v8::Value> err = NanNewLocal<v8::Value>(v8::Null());
    v8::Local<v8::Value> val;
    if (value) {
      val = NanNewLocal<v8::String>(v8::String::New(value));
    } else {
      val = NanNewLocal<v8::Value>(v8::Null());
    }
    v8::Local<v8::Value> argv[2] = { err, val };
    callback->Call(2, argv);
    delete value;
  }

private:
  Sophist *self;
  char *key;
  char *value;
};

/**
 * Async set worker.
 */

class SetWorker : public NanAsyncWorker {
public:
  SetWorker(
      Sophist *self
    , char *key
    , char *value
    , NanCallback *callback
  ) : NanAsyncWorker(callback)
    , self(self)
    , key(key)
    , value(value) {}

  void
  Execute() {
    sophia::SophiaReturnCode rc = self->sp->Set(key, value);
    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

private:
  Sophist *self;
  char *key;
  char *value;
};

/**
 * Async delete worker.
 */

class DeleteWorker : public NanAsyncWorker {
public:
  DeleteWorker(
      Sophist *self
    , char *key
    , NanCallback *callback
  ) : NanAsyncWorker(callback)
    , self(self)
    , key(key) {}

  void
  Execute() {
    sophia::SophiaReturnCode rc = self->sp->Delete(key);
    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

private:
  Sophist *self;
  char *key;
};

/**
 * Async clear worker.
 */

class ClearWorker : public NanAsyncWorker {
public:
  ClearWorker(
      Sophist *self
    , NanCallback *callback
  ) : NanAsyncWorker(callback), self(self) {}

  void
  Execute() {
    sophia::SophiaReturnCode rc = self->sp->Clear();
    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

private:
  Sophist *self;
};

/**
 * Async clear worker.
 */

class CountWorker : public NanAsyncWorker {
public:
  CountWorker(
      Sophist *self
    , NanCallback *callback
  ) : NanAsyncWorker(callback), self(self) {}

  void
  Execute() {
    sophia::SophiaReturnCode rc = self->sp->Count(&count);
    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

  void
  HandleOKCallback() {
    v8::Local<v8::Value> argv[] = {
        NanNewLocal<v8::Value>(v8::Null())
      , NanNewLocal<v8::Number>(v8::Number::New(count))
    };
    callback->Call(2, argv);
  }

private:
  Sophist *self;
  size_t count;
};

Sophist::Sophist(const char *path) : path(path) {
  sp = new sophia::Sophia(path);
}

Sophist::~Sophist() {
  delete path;
}

void
Sophist::Init(v8::Handle<v8::Object> exports) {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(
    Sophist::New
  );
  NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);

  tpl->SetClassName(NanSymbol("Sophist"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSync", GetSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set", Set);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setSync", SetSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);
  NODE_SET_PROTOTYPE_METHOD(tpl, "deleteSync", DeleteSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "count", Count);
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
  NODE_SET_PROTOTYPE_METHOD(tpl, "iterator", NewIterator);
  NODE_SET_PROTOTYPE_METHOD(tpl, "transaction", NewTransaction);

  exports->Set(NanSymbol("Sophist"), tpl->GetFunction());
}

NAN_METHOD(Sophist::New) {
  NanScope();
  size_t pathsize;
  const char *path = NanCString(args[0], &pathsize);
  Sophist *self = new Sophist(path);
  self->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(Sophist::Open) {
  NanScope();
  Sophist *self = node::ObjectWrap::Unwrap<Sophist>(args.This());
  v8::Local<v8::Object> options = args[0].As<v8::Object>();
  v8::Local<v8::Function> fn = args[1].As<v8::Function>();
  bool create_if_missing = NanBooleanOptionValue(
      options
    , NanSymbol("createIfMissing")
    , true
  );
  bool read_only = NanBooleanOptionValue(
      options
    , NanSymbol("readOnly")
    , false
  );
  uint32_t merge_watermark = NanUInt32OptionValue(
      options
    , NanSymbol("mergeWatermark")
    , 100000
  );
  uint32_t page_size = NanUInt32OptionValue(
      options
    , NanSymbol("pageSize")
    , 2048
  );

  OpenWorker *worker = new OpenWorker(
      self
    , create_if_missing
    , read_only
    , merge_watermark
    , page_size
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Sophist::Close) {
  NanScope();
  Sophist *self = node::ObjectWrap::Unwrap<Sophist>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  CloseWorker *worker = new CloseWorker(self, new NanCallback(fn));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Sophist::Get) {
  NanScope();
  Sophist *self = node::ObjectWrap::Unwrap<Sophist>(args.This());
  size_t keysize;
  char *key = NanCString(args[0], &keysize);
  v8::Local<v8::Function> fn = args[1].As<v8::Function>();
  // TODO support `Sophist#get('my key', 1234, function (err, res))`
  GetWorker *worker = new GetWorker(
      self
    , key
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Sophist::GetSync) {
  NanScope();
  Sophist *self = node::ObjectWrap::Unwrap<Sophist>(args.This());
  size_t keysize;
  char *key = NanCString(args[0], &keysize);
  char *value = self->sp->Get(key, keysize);
  delete key;
  if (NULL == value) {
    NanReturnValue(v8::Null());
  } else {
    NanReturnValue(v8::String::New(value));
  }
}

NAN_METHOD(Sophist::Set) {
  NanScope();
  Sophist *self = ObjectWrap::Unwrap<Sophist>(args.This());
  size_t keysize;
  size_t valuesize;
  char *key = NanCString(args[0], &keysize);
  char *value = NanCString(args[1], &valuesize);
  v8::Local<v8::Function> fn = args[2].As<v8::Function>();
  SetWorker *worker = new SetWorker(
      self
    , key
    , value
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Sophist::SetSync) {
  NanScope();
  Sophist *self = ObjectWrap::Unwrap<Sophist>(args.This());
  size_t keysize;
  size_t valuesize;
  char *key = NanCString(args[0], &keysize);
  char *value = NanCString(args[1], &valuesize);
  sophia::SophiaReturnCode rc = self->sp->Set(
      key
    , value
  );
  if (sophia::SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sp->Error(rc));
  }
  NanReturnUndefined();
}

NAN_METHOD(Sophist::Delete) {
  NanScope();
  Sophist *self = ObjectWrap::Unwrap<Sophist>(args.This());
  size_t keysize;
  char *key = NanCString(args[0], &keysize);
  v8::Local<v8::Function> fn = args[1].As<v8::Function>();
  DeleteWorker *worker = new DeleteWorker(
      self
    , key
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Sophist::DeleteSync) {
  NanScope();
  Sophist *self = ObjectWrap::Unwrap<Sophist>(args.This());
  size_t keysize;
  char *key = NanCString(args[0], &keysize);
  sophia::SophiaReturnCode rc = self->sp->Delete(key);
  if (sophia::SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sp->Error(rc));
  }
  NanReturnUndefined();
}

NAN_METHOD(Sophist::Count) {
  NanScope();
  Sophist *self = ObjectWrap::Unwrap<Sophist>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  CountWorker *worker = new CountWorker(self, new NanCallback(fn));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Sophist::Clear) {
  NanScope();
  Sophist *self = ObjectWrap::Unwrap<Sophist>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  ClearWorker *worker = new ClearWorker(self, new NanCallback(fn));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Sophist::NewIterator) {
  NanScope();

  v8::Local<v8::Object> options;
  if (args.Length() > 0 && args[0]->IsObject()) {
    options = v8::Local<v8::Object>::Cast(args[0]);
  }

  NanReturnValue(Iterator::NewInstance(
      args.This()
    , options
  ));
}

NAN_METHOD(Sophist::NewTransaction) {
  NanScope();
  NanReturnValue(Transaction::NewInstance(args.This()));
}

} // namespace sophist
