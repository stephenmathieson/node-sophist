
#include <string.h>
#include <node.h>
#include <nan.h>
#include <sophia.h>

namespace sophist {
  class SetWorker : public NanAsyncWorker {
    public:
      SetWorker(void *db, char *key, char *value, NanCallback *callback)
        : NanAsyncWorker(callback)
        , db(db)
        , key(key)
        , value(value) {}

      void Execute() {
        size_t keysize = strlen(key) + 1;
        size_t valuesize = strlen(value) + 1;
        int rc = sp_set(db, key, keysize, value, valuesize);
        if (-1 == rc) errmsg = strdup(sp_error(db));
      }

    private:
      void *db;
      char *key;
      char *value;
  };

  void Set(void *db, char *key, char *value, NanCallback *callback) {
    SetWorker *worker = new SetWorker(db, key, value, callback);
    NanAsyncQueueWorker(worker);
  }
}
