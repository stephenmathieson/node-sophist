
#ifndef SOPHIST_TRANSACTION_H
#define SOPHIST_TRANSACTION_H 1

#include <node.h>
#include <nan.h>
#include <list.h>
#include "sophist.h"

namespace sophist {

  enum OperationType {
      OPERATION_SET = 0
    , OPERATION_DELETE = 1
  };

  struct Operation {
    char *key;
    char *value;
    OperationType type;
  };

  class Transaction : public node::ObjectWrap {
    public:
      void *db;
      list_t *operations;

      static void Init();
      static v8::Local<v8::Object> NewInstance(v8::Local<v8::Object>);

    private:
      Transaction();
      ~Transaction();

      static v8::Handle<v8::Value> New(const v8::Arguments &args);
      static NAN_METHOD(Commit);
      static NAN_METHOD(Rollback);
      static NAN_METHOD(Set);
      static NAN_METHOD(Delete);
  };
}

#endif
