
#include <node.h>
#include "sophist.h"
#include "iterator.h"
#include "transaction.h"

void InitAll(v8::Handle<v8::Object> exports) {
  sophist::Iterator::Init();
  sophist::Transaction::Init();
  sophist::Sophist::Init(exports);
}

NODE_MODULE(sophist, InitAll)
