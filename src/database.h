
#ifndef DATABASE_H
#define DATABASE_H 1

#include <map>
#include <node.h>
#include <nan.h>
#include "sophia-cc.h"
#include "iterator.h"

namespace sophist {

class Database : public node::ObjectWrap {
public:
  Database(char *path);
  ~Database();

  static void Init(v8::Handle<v8::Object> exports);
  void ReleaseIterator(uint32_t id);

  char *path;
  sophia::Sophia *sophia;
  std::map<uint32_t, class Iterator *> iterators;

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

  uint32_t currentIteratorId;
};

}; // namespace sophist

#endif
