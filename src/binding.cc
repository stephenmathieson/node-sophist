
#include <node.h>
#include "sophist.h"

using namespace v8;
using namespace sophist;

void InitAll(Handle<Object> exports) {
  Sophist::Init(exports);
}

NODE_MODULE(sophist, InitAll)
