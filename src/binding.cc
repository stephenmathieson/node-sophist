
#include <node.h>
#include "sophist.h"
#include "iterator.h"

using namespace v8;
using namespace sophist;

void InitAll(Handle<Object> exports) {
  Iterator::Init();
  Sophist::Init(exports);
}

NODE_MODULE(sophist, InitAll)
