
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "sophist.h"
#include "iterator.h"
#include "iterator_next.h"
#include "iterator_end.h"

using namespace v8;

namespace sophist {
  static Persistent<FunctionTemplate> iterator_constructor;

  Iterator::Iterator() {
    endsize = 0;
    end = NULL;
  }

  Iterator::~Iterator() {
    if (end) delete end;
  }

  void Iterator::Init () {
    Local<FunctionTemplate> tpl =
      FunctionTemplate::New(Iterator::New);
    NanAssignPersistent(FunctionTemplate, iterator_constructor, tpl);
    tpl->SetClassName(NanSymbol("Iterator"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "next", Iterator::Next);
    NODE_SET_PROTOTYPE_METHOD(tpl, "end", Iterator::End);
  }

  NAN_METHOD(Iterator::New) {
    NanScope();
    Iterator *iterator = new Iterator();
    Sophist *sp = ObjectWrap::Unwrap<Sophist>(args[0]->ToObject());
    if (NULL == sp->db) {
      return NanThrowError("Unable to create an iterator "
                           "on an unopen database!");
    }

    iterator->Wrap(args.This());
    iterator->wrapper = sp;

    Local<Object> options = Local<Object>::Cast(args[1]);
    bool reverse = NanBooleanOptionValue(options
      , NanSymbol("reverse")
      , false);

    bool gte = NanBooleanOptionValue(options
      , NanSymbol("gte")
      , false);

    char *start = NULL;
    size_t startsize = 0;

    if (options->Has(NanSymbol("start"))) {
      start = NanCString(options->Get(NanSymbol("start")).As<String>()
        , &startsize);
    }

    if (options->Has(NanSymbol("end"))) {
      iterator->end = NanCString(options->Get(NanSymbol("end")).As<String>()
        , &iterator->endsize);
      // HACK: compensate for strlen(key) + 1
      // see pmwkaa/sophia#43
      iterator->endsize++;
    }

    iterator->order = true == reverse
      ? SPLT
      : gte
        ? SPGTE
        : SPGT;
    iterator->cursor = sp_cursor(sp->db
      , iterator->order
      , start
      , startsize);

    delete start;

    if (NULL == iterator->cursor) {
      return NanThrowError(sp_error(sp->db));
    }

    NanReturnValue(args.This());
  }

  NAN_METHOD(Iterator::Next) {
    NanScope();
    Iterator *iterator = ObjectWrap::Unwrap<Iterator>(args.This());
    Local<Function> cb = args[0].As<Function>();
    sophist::IteratorNext(iterator, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Iterator::End) {
    NanScope();
    Iterator *iterator = ObjectWrap::Unwrap<Iterator>(args.This());
    Local<Function> cb = args[0].As<Function>();
    sophist::IteratorEnd(iterator, new NanCallback(cb));
    NanReturnUndefined();
  }

  Local<Object>
  Iterator::NewInstance(Local<Object> sp, Local<Object> options) {
    NanScope();
    Local<Object> instance;
    Local<FunctionTemplate> constructorHandle =
      NanPersistentToLocal(iterator_constructor);
    Handle<Value> argv[2] = {
        sp
      , options
    };
    instance = constructorHandle->GetFunction()->NewInstance(2, argv);
    return instance;
  }
}
