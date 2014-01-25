
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

using namespace v8;

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
    NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);
    NODE_SET_PROTOTYPE_METHOD(tpl, "set", Set);
    NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);
    NODE_SET_PROTOTYPE_METHOD(tpl, "count", Count);
    NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
    NODE_SET_PROTOTYPE_METHOD(tpl, "iterator", NewIterator);
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
    Local<Object> options = args[0].As<Object>();
    Local<Function> cb = args[1].As<Function>();
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

  NAN_METHOD(Sophist::NewIterator) {
    NanScope();

    Local<Object> options;
    if (args.Length() > 0 && args[0]->IsObject()) {
      options = Local<Object>::Cast(args[0]);
    }

    NanReturnValue(Iterator::NewInstance(
        args.This()
      , options
    ));
  }
}
