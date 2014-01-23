
#include <string.h>
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "delete.h"

namespace sophist {
  class DeleteWorker : public NanAsyncWorker {
    public:
      DeleteWorker(void *db, char *key, NanCallback *callback)
        : NanAsyncWorker(callback)
        , db(db)
        , key(key) {}

      void Execute() {
        size_t keysize = strlen(key) + 1;
        if (-1 == sp_delete(db, key, keysize)) errmsg = sp_error(db);
      }

    private:
      void *db;
      char *key;
  };

  void Delete(void *db, char *key, NanCallback* callback) {
    DeleteWorker *worker = new DeleteWorker(db, key, callback);
    NanAsyncQueueWorker(worker);
  }
}
