
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
#include "iterator.h"

namespace sophist {
  Sophist::Sophist() {
    db = NULL;
    env = NULL;
    path = NULL;
  }

  Sophist::~Sophist() {}

  void Sophist::Init(v8::Handle<v8::Object> exports) {
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
    tpl->SetClassName(NanSymbol("Sophist"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getSync", GetSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "set", Set);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setSync", SetSync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);
    NODE_SET_PROTOTYPE_METHOD(tpl, "count", Count);
    NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
    NODE_SET_PROTOTYPE_METHOD(tpl, "iterator", NewIterator);
    v8::Persistent<v8::Function> constructor =
      v8::Persistent<v8::Function>::New(tpl->GetFunction());
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
    Sophist *wrapper = node::ObjectWrap::Unwrap<Sophist>(args.This());
    v8::Local<v8::Object> options = args[0].As<v8::Object>();
    v8::Local<v8::Function> cb = args[1].As<v8::Function>();
    bool create = NanBooleanOptionValue(options
      , NanSymbol("createIfMissing")
      , true);
    bool read_only = NanBooleanOptionValue(options
      , NanSymbol("readOnly")
      , false);
    bool gc = NanBooleanOptionValue(options
      , NanSymbol("gc")
      , true);
    uint32_t merge_watermark = NanUInt32OptionValue(options
      , NanSymbol("mergeWatermark")
      , 100000);
    uint32_t page_size = NanUInt32OptionValue(options
      , NanSymbol("pageSize")
      , 2048);

    sophist::Open(wrapper
      , create
      , read_only
      , gc
      , merge_watermark
      , page_size
      , new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Close) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    v8::Local<v8::Function> cb = args[0].As<v8::Function>();
    sophist::Close(wrapper, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Get) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    size_t keysize;
    char *key = NanCString(args[0], &keysize);
    v8::Local<v8::Function> cb = args[1].As<v8::Function>();
    sophist::Get(wrapper->db, key, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::GetSync) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    size_t keysize;
    char *key = NanCString(args[0], &keysize);
    char *value = sophist::GetSync(wrapper->db, key);
    if (NULL == value) {
      NanReturnValue(v8::Null());
    } else {
      NanReturnValue(v8::String::New(value));
    }
    delete key;
    delete value;
  }

  NAN_METHOD(Sophist::Set) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    size_t keysize;
    size_t valuesize;
    char *key = NanCString(args[0], &keysize);
    char *value = NanCString(args[1], &valuesize);
    v8::Local<v8::Function> cb = args[2].As<v8::Function>();
    sophist::Set(wrapper->db, key, value, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::SetSync) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    size_t keysize;
    size_t valuesize;
    char *key = NanCString(args[0], &keysize);
    char *value = NanCString(args[1], &valuesize);
    sophist::SetSync(wrapper->db, key, value);
    NanReturnUndefined();
    delete key;
    delete value;
  }

  NAN_METHOD(Sophist::Delete) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    size_t keysize;
    char *key = NanCString(args[0], &keysize);
    v8::Local<v8::Function> cb = args[1].As<v8::Function>();
    sophist::Delete(wrapper->db, key, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Count) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    v8::Local<v8::Function> cb = args[0].As<v8::Function>();
    sophist::Count(wrapper->db, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Sophist::Clear) {
    NanScope();
    Sophist *wrapper = ObjectWrap::Unwrap<Sophist>(args.This());
    v8::Local<v8::Function> cb = args[0].As<v8::Function>();
    sophist::Clear(wrapper->db, new NanCallback(cb));
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
}
