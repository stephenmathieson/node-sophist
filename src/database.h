
#ifndef DATABASE_H
#define DATABASE_H 1

#include <node.h>
#include <nan.h>
#include "sophia-cc.h"

namespace sophist {

class Database : public node::ObjectWrap {
public:
  Database(char *path);
  ~Database();

  static void Init(v8::Handle<v8::Object> exports);

  char *path;
  sophia::Sophia *sophia;

private:
  static NAN_METHOD(New);
  static NAN_METHOD(Open);
  static NAN_METHOD(OpenSync);
  static NAN_METHOD(Close);
  static NAN_METHOD(CloseSync);
  static NAN_METHOD(Set);
  static NAN_METHOD(SetSync);
  static NAN_METHOD(Get);
  static NAN_METHOD(GetSync);
  static NAN_METHOD(Delete);
  static NAN_METHOD(DeleteSync);
  static NAN_METHOD(Iterator);
};

}; // namespace sophist

#endif
