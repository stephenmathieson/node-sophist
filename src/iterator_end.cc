
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "iterator.h"
#include "iterator_end.h"

namespace sophist {
  class IteratorEndWorker : public NanAsyncWorker {
    public:
      IteratorEndWorker(Iterator *iterator, NanCallback *callback)
        : NanAsyncWorker(callback) {
          cursor = iterator->cursor;
        }

      void Execute() {
        if (-1 == sp_destroy(cursor))
          errmsg = strdup("Unable to close cursor");
      }

    private:
      void *cursor;
  };

  void IteratorEnd(Iterator *iterator, NanCallback *callback) {
    IteratorEndWorker *worker = new IteratorEndWorker(iterator, callback);
    NanAsyncQueueWorker(worker);
  }
}
