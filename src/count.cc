
#include <string.h>
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "count.h"

namespace sophist {
  class CountWorker : public NanAsyncWorker {
    public:
      CountWorker(void *db, NanCallback *callback)
        : NanAsyncWorker(callback)
        , db(db) {
          count = 0;
        }

      void Execute() {
        void *c = sp_cursor(db, SPGT, NULL, 0);
        if (NULL == c) {
          errmsg = sp_error(db);
          return;
        }

        while (sp_fetch(c))
          if (sp_key(c)) count++;

        sp_destroy(c);
      }

      void HandleOKCallback() {
        v8::Local<v8::Value> argv[] = {
            v8::Local<v8::Value>::New(v8::Null())
          , v8::Local<v8::Value>::New(v8::Number::New(count))
        };
        callback->Call(2, argv);
      }

    private:
      void *db;
      int count;
  };

  void Count(void *db, NanCallback *callback) {
    CountWorker *worker = new CountWorker(db, callback);
    NanAsyncQueueWorker(worker);
  }
}
