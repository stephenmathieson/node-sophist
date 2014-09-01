
#include "database.h"
#include "iterator.h"
#include "transaction.h"

void InitSophist(v8::Handle<v8::Object> exports) {
  sophist::Database::Init(exports);
  sophist::Iterator::Init();
  sophist::Transaction::Init();
}

NODE_MODULE(sophist, InitSophist);
