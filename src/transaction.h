
#ifndef NODE_SOPHIST_TRANSACTION_H
#define NODE_SOPHIST_TRANSACTION_H 1

#include <node.h>
#include <sophia-cc.h>
#include "sophist.h"

namespace sophist {

class Transaction : public node::ObjectWrap {
public:
  Sophist *sophist;
  sophia::Sophia *sp;
  sophia::Transaction *t;

  static v8::Persistent<v8::FunctionTemplate> constructor;

  static void
  Init();

  static v8::Local<v8::Object>
  NewInstance(v8::Local<v8::Object>);

private:
  Transaction();
  ~Transaction();

  static NAN_METHOD(New);
  static NAN_METHOD(Commit);
  static NAN_METHOD(Rollback);
  static NAN_METHOD(Set);
  static NAN_METHOD(Delete);
};

} // namespace sophist

#endif
