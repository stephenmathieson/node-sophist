
#include <string.h>
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "get.h"

namespace sophist {
  class GetWorker : public NanAsyncWorker {
    public:
      GetWorker(void *db, char *key, NanCallback *callback)
        : NanAsyncWorker(callback)
        , db(db)
        , key(key) {
          value = NULL;
        }

      void Execute() {
        void *ref = NULL;
        int rc = 0;
        size_t valuesize = 0;
        size_t keysize = strlen(key) + 1;

        rc = sp_get(db, key, keysize, &ref, &valuesize);
        if (1 != rc) {
          errmsg = sp_error(db);
          return;
        }

        // only cast if the key exists
        if (NULL == ref) return;

        value = (char *) ref;
        valuesize--;
        if ('\0' != value[valuesize]) value[valuesize] = '\0';
      }

      void HandleOKCallback() {
        v8::Local<v8::Value> returnValue;;

        if (NULL == value) {
          returnValue = v8::Local<v8::Value>::New(v8::Null());
        } else {
          returnValue = v8::Local<v8::Value>::New(v8::String::New(value));
        }

        v8::Local<v8::Value> argv[] = {
          v8::Local<v8::Value>::New(v8::Null()),
          returnValue
        };
        callback->Call(2, argv);
      }

    private:
      void *db;
      char *key;
      char *value;
  };

  void Get(void *db, char *key, NanCallback* callback) {
    GetWorker *worker = new GetWorker(db, key, callback);
    NanAsyncQueueWorker(worker);
  }

  char *GetSync(void *db, char *key) {
    void *ref = NULL;
    char *value = NULL;
    int rc = 0;
    size_t valuesize = 0;
    size_t keysize = strlen(key) + 1;

    rc = sp_get(db, key, keysize, &ref, &valuesize);
    if (-1 == rc) {
      NanThrowError(sp_error(db));
      return NULL;
    }

    // only cast if the key exists
    if (0 == rc || NULL == ref) return NULL;

    value = (char *) ref;
    valuesize--;
    if ('\0' != value[valuesize]) value[valuesize] = '\0';
    return value;
  }
}
