
#ifndef SOPHIST_H
#define SOPHIST_H 1

#include <node.h>
#include <nan.h>
#include <sophia.h>

namespace sophist {
  class Sophist : public node::ObjectWrap {
    public:
      void *db;
      void *env;
      char *path;
      static void Init(v8::Handle<v8::Object> exports);

    private:
      Sophist();
      ~Sophist();

      static v8::Handle<v8::Value> New(const v8::Arguments& args);
      static NAN_METHOD(Open);
      static NAN_METHOD(Close);
      static NAN_METHOD(Get);
      static NAN_METHOD(Set);
      static NAN_METHOD(Delete);
      static NAN_METHOD(Count);
      static NAN_METHOD(Clear);
  };
}

#endif
