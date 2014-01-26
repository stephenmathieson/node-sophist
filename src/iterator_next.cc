
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "iterator.h"
#include "iterator_next.h"

namespace sophist {
  class IteratorNextWorker : public NanAsyncWorker {
    public:
      IteratorNextWorker(Iterator *iterator, NanCallback *callback)
        : NanAsyncWorker(callback) {
          cursor = iterator->cursor;
          db = iterator->wrapper->db;
          key = NULL;
          value = NULL;
          end = iterator->end;
          endsize = iterator->endsize;
        }

      void Execute() {
        size_t ksize = 0;
        size_t vsize = 0;
        const char *k;
        const char *v;

        // out of keys?
        if (0 == sp_fetch(cursor)) return;

        if (!(k = sp_key(cursor)) || !(v = sp_value(cursor))) {
          errmsg = strdup(sp_error(db));
          return;
        }

        ksize = sp_keysize(cursor);
        vsize = sp_valuesize(cursor);
        // at the "end" key?
        if (end && endsize == ksize && 0 == strncmp(end, k, ksize)) {
          return;
        }

        key = strdup(k);
        value = strdup(v);

        if ('\0' != key[ksize]) key[ksize] = '\0';
        if ('\0' != value[vsize]) value[vsize] = '\0';
      }

      void HandleOKCallback() {
        if (key && value) {
          v8::Local<v8::Value> argv[3] = {
            v8::Local<v8::Value>::New(v8::Null()),
            v8::Local<v8::Value>::New(v8::String::New(key)),
            v8::Local<v8::Value>::New(v8::String::New(value))
          };
          callback->Call(3, argv);
        } else {
          v8::Local<v8::Value> argv[3] = {
            v8::Local<v8::Value>::New(v8::Null()),
            v8::Local<v8::Value>::New(v8::Null()),
            v8::Local<v8::Value>::New(v8::Null())
          };
          callback->Call(3, argv);
        }
      }

    private:
      void *cursor;
      void *db;
      char *end;
      size_t endsize;
      char *key;
      char *value;
  };

  void IteratorNext(Iterator *iterator, NanCallback *callback) {
    IteratorNextWorker *worker = new IteratorNextWorker(iterator, callback);
    NanAsyncQueueWorker(worker);
  }
}
