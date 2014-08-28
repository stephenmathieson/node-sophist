
#include <nan.h>
#include "sophia-cc.h"
#include "iterator_workers.h"

#include <stdio.h>

using namespace sophia;

namespace sophist {

NextWorker::NextWorker(
    Iterator *iterator
  , NanCallback *callback
) : IteratorWorker(iterator, callback) {}

void NextWorker::Execute() {
  result = iterator->it->Next();
}

void NextWorker::HandleOKCallback() {
  if (result) {
    v8::Local<v8::Value> argv[] = {
        NanNull()
      , NanNew<v8::String>(result->key)
      , NanNew<v8::String>(result->value)
    };
    delete result;
    callback->Call(3, argv);
  } else {
    v8::Local<v8::Value> argv[] = { NanNull() };
    callback->Call(1, argv);
  }
}

EndWorker::EndWorker(
    Iterator *iterator
  , NanCallback *callback
) : IteratorWorker(iterator, callback) {}

void EndWorker::Execute() {
  sophia::SophiaReturnCode rc = iterator->it->End();
  if (SOPHIA_SUCCESS != rc) {
    SetErrorMessage(iterator->database->sophia->Error(rc));
  }
}

} // namespace sophist
