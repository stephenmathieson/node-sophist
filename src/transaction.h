
#ifndef SOPHIST_TRANSACTION_H
#define SOPHIST_TRANSACTION_H 1

#include <node.h>
#include <nan.h>
#include "sophist.h"

namespace sophist {
  class Transaction : public node::ObjectWrap {
    public:
      void *db;

      static void Init();
      static v8::Local<v8::Object> NewInstance(v8::Local<v8::Object>);

    private:
      Transaction();
      ~Transaction();

      static v8::Handle<v8::Value> New(const v8::Arguments &args);
      static NAN_METHOD(Commit);
      static NAN_METHOD(Rollback);
  };
}

#endif
