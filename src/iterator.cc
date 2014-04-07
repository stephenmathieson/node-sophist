
#include <node.h>
#include <nan.h>
#include "sophist.h"
#include "iterator.h"

namespace sophist {

v8::Persistent<v8::FunctionTemplate> Iterator::constructor;

/**
 * Async next worker.
 */

class NextWorker : public NanAsyncWorker {
public:
  NextWorker(Iterator *self, NanCallback *callback)
  : NanAsyncWorker(callback), self(self) {
    result = NULL;
  };

  void
  Execute() {
    result = self->it->Next();
  }

  void
  HandleOKCallback() {
    if (result) {
      v8::Local<v8::Value> argv[] = {
          NanNewLocal<v8::Value>(v8::Null())
        , NanNewLocal<v8::String>(v8::String::New(result->key))
        , NanNewLocal<v8::String>(v8::String::New(result->value))
      };
      delete result;
      callback->Call(3, argv);
    } else {
      v8::Local<v8::Value> argv[] = {
        NanNewLocal<v8::Value>(v8::Null())
      };
      callback->Call(1, argv);
    }
  }

private:
  sophia::IteratorResult *result;
  Iterator *self;
};

/**
 * Async end worker.
 */

class EndWorker : public NanAsyncWorker {
public:
  EndWorker(Iterator *self, NanCallback *callback)
  : NanAsyncWorker(callback), self(self) {}

  void
  Execute() {
    sophia::SophiaReturnCode rc = self->it->End();
    if (sophia::SOPHIA_SUCCESS != rc) {
      errmsg = strdup(self->sp->Error(rc));
    }
  }

private:
  Iterator *self;
};

Iterator::Iterator() {};

Iterator::~Iterator() {};

void
Iterator::Init() {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(
    Iterator::New
  );
  NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);
  tpl->SetClassName(NanSymbol("Iterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "end", End);
}

v8::Local<v8::Object>
Iterator::NewInstance(
    v8::Local<v8::Object> sophist
  , v8::Local<v8::Object> options
) {
  NanScope();
  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> c = NanPersistentToLocal(
    constructor
  );
  v8::Handle<v8::Value> argv[2] = { sophist, options };
  instance = c->GetFunction()->NewInstance(2, argv);
  return instance;
}

NAN_METHOD(Iterator::New) {
  NanScope();
  Sophist *sophist = ObjectWrap::Unwrap<Sophist>(
    args[0]->ToObject()
  );
  v8::Local<v8::Object> options = v8::Local<v8::Object>::Cast(args[1]);

  if (!sophist->sp->IsOpen()) {
    return NanThrowError(
      "Cannot create an iterator on an unopen database"
    );
  }

  Iterator *self = new Iterator;
  self->Wrap(args.This());
  self->sophist = sophist;
  self->sp = sophist->sp;

  bool reverse = NanBooleanOptionValue(
      options
    , NanSymbol("reverse")
    , false
  );

  bool gte = NanBooleanOptionValue(
      options
    , NanSymbol("gte")
    , false
  );

  const char *start = NULL;
  const char *end = NULL;
  size_t startsize = 0;
  size_t endsize = 0;

  if (options->Has(NanSymbol("start"))) {
    start = NanCString(
        options->Get(NanSymbol("start")).As<v8::String>()
      , &startsize
    );
  }

  if (options->Has(NanSymbol("end"))) {
    end = NanCString(
        options->Get(NanSymbol("end")).As<v8::String>()
      , &endsize
    );
  }

  self->it = new sophia::Iterator(
      self->sp
    , reverse ? SPLT : gte ? SPGTE : SPGT
    , start
    , start ? startsize + 1 : 0
    , end
    , end ? endsize + 1 : 0
  );

  sophia::SophiaReturnCode rc = self->it->Begin();
  if (sophia::SOPHIA_SUCCESS != rc) {
    NanThrowError(self->sp->Error(rc));
  }
  NanReturnValue(args.This());
}

NAN_METHOD(Iterator::Next) {
  NanScope();
  Iterator *self = ObjectWrap::Unwrap<Iterator>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  NextWorker *worker = new NextWorker(self, new NanCallback(fn));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(Iterator::End) {
  NanScope();
  Iterator *self = ObjectWrap::Unwrap<Iterator>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  EndWorker *worker = new EndWorker(self, new NanCallback(fn));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

} // namespace sophist
