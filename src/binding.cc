
#include "database.h"
#include "iterator.h"

void InitSophist(v8::Handle<v8::Object> exports) {
  sophist::Database::Init(exports);
  sophist::Iterator::Init();
}

NODE_MODULE(sophist, InitSophist);
