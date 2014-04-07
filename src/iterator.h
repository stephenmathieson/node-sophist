
#ifndef NODE_SOPHIST_ITERATOR_H
#define NODE_SOPHIST_ITERATOR_H 1

#include <node.h>
#include <nan.h>
#include <sophia-cc.h>
#include "sophist.h"

namespace sophist {

class Iterator : public node::ObjectWrap {
public:
  Sophist *sophist;
  sophia::Sophia *sp;
  sophia::Iterator *it;

  static v8::Persistent<v8::FunctionTemplate> constructor;

  static void
  Init();

  static v8::Local<v8::Object>
  NewInstance(v8::Local<v8::Object>, v8::Local<v8::Object>);

private:
  Iterator();
  ~Iterator();

  static NAN_METHOD(New);
  static NAN_METHOD(Next);
  static NAN_METHOD(End);

};

} // namespace sophist

#endif
