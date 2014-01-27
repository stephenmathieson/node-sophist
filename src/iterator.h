
#ifndef SOPHIST_ITERATOR_H
#define SOPHIST_ITERATOR_H 1

#include <node.h>
#include <nan.h>
#include "sophist.h"

namespace sophist {
  class Iterator : public node::ObjectWrap {
    public:
      Sophist *wrapper;
      void *cursor;
      size_t endsize;
      char *end;
      sporder order;

      static void Init();

      static v8::Local<v8::Object> NewInstance(
          v8::Local<v8::Object>
        , v8::Local<v8::Object>
      );

    private:
      Iterator();
      ~Iterator();

      static v8::Handle<v8::Value> New(const v8::Arguments &args);

      static NAN_METHOD(Next);
      static NAN_METHOD(End);
  };
};

#endif
