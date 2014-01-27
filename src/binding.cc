
#include <node.h>
#include "sophist.h"
#include "iterator.h"
#include "transaction.h"

using namespace sophist;

void InitAll(v8::Handle<v8::Object> exports) {
  Iterator::Init();
  Transaction::Init();
  Sophist::Init(exports);
}

NODE_MODULE(sophist, InitAll)
