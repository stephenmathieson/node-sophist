
#ifndef NODE_SOPHIST_H
#define NODE_SOPHIST_H 1

#include <node.h>
#include <nan.h>
#include "sophia-cc.h"

namespace sophist {

class Sophist : public node::ObjectWrap {
public:
  sophia::Sophia *sp;

  static void
  Init(v8::Handle<v8::Object> exports);

  static v8::Persistent<v8::FunctionTemplate> constructor;

private:
  Sophist(const char *path);
  ~Sophist();

  static NAN_METHOD(New);
  static NAN_METHOD(Open);
  static NAN_METHOD(Close);
  static NAN_METHOD(Get);
  static NAN_METHOD(GetSync);
  static NAN_METHOD(Set);
  static NAN_METHOD(SetSync);
  static NAN_METHOD(Delete);
  static NAN_METHOD(DeleteSync);
  static NAN_METHOD(Count);
  static NAN_METHOD(Clear);
  static NAN_METHOD(NewIterator);
  static NAN_METHOD(NewTransaction);

  const char *path;
};


};

#endif
