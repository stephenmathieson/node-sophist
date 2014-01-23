
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "sophist.h"
#include "open.h"
#include "close.h"
#include "get.h"
#include "set.h"
#include "delete.h"
#include "count.h"
#include "clear.h"

using namespace v8;

#define PROTOTYPE_METHOD(prototype, name, fn)             \
  prototype                                               \
    ->PrototypeTemplate()                                 \
      ->Set(                                              \
            NanSymbol(name)                               \
          , v8::FunctionTemplate::New(fn)->GetFunction()  \
        );

namespace sophist {
  Sophist::Sophist() {
    db = NULL;
    env = NULL;
    path = NULL;
  }

  Sophist::~Sophist() {}

  void Sophist::Init(Handle<Object> exports) {
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(NanSymbol("Sophist"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    PROTOTYPE_METHOD(tpl, "open", Open);
    PROTOTYPE_METHOD(tpl, "close", Close);
    PROTOTYPE_METHOD(tpl, "get", Get);
    PROTOTYPE_METHOD(tpl, "set", Set);
    PROTOTYPE_METHOD(tpl, "delete", Delete);
    PROTOTYPE_METHOD(tpl, "count", Count);
    PROTOTYPE_METHOD(tpl, "clear", Clear);
    Persistent<Function> constructor =
      Persistent<Function>::New(tpl->GetFunction());
    exports->Set(NanSymbol("Sophist"), constructor);
  }

  NAN_METHOD(Sophist::New) {
    NanScope();
    Sophist *wrapper = new Sophist();
    size_t size;
    wrapper->path = NanCString(args[0], &size);
    wrapper->Wrap(args.This());
    NanReturnValue(args.This());
  }

  NAN_METHOD(Sophist::Open) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    Local<Function> cb = args[0].As<Function>();
    sophist::Open(wrapper, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Close) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    Local<Function> cb = args[0].As<Function>();
    sophist::Close(wrapper, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Get) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    size_t keysize;
    char *key = NanCString(args[0], &keysize);
    Local<Function> cb = args[1].As<Function>();
    sophist::Get(wrapper->db, key, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Set) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    size_t keysize;
    size_t valuesize;
    char *key = NanCString(args[0], &keysize);
    char *value = NanCString(args[1], &valuesize);
    Local<Function> cb = args[2].As<Function>();
    sophist::Set(wrapper->db, key, value, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Delete) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    size_t keysize;
    char *key = NanCString(args[0], &keysize);
    Local<Function> cb = args[1].As<Function>();
    sophist::Delete(wrapper->db, key, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Count) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    Local<Function> cb = args[0].As<Function>();
    sophist::Count(wrapper->db, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Clear) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    Local<Function> cb = args[0].As<Function>();
    sophist::Clear(wrapper->db, new NanCallback(cb));
    NanReturnUndefined();
  }
}
