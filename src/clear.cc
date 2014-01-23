
#include <string.h>
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "clear.h"

namespace sophist {
  class ClearWorker : public NanAsyncWorker {
    public:
      ClearWorker(void *db, NanCallback *callback)
        : NanAsyncWorker(callback)
        , db(db) {}

      // TODO this is nasty: waaayyy too much
      // iteration here
      void Execute() {
        int count = 0;
        void *c = sp_cursor(db, SPGT, NULL, 0);
        if (NULL == c) {
          errmsg = sp_error(db);
          return;
        }

        while (sp_fetch(c))
          if (sp_key(c)) count++;

        sp_destroy(c);

        if (0 == count) return;

        char *keys[count];
        int index = 0;
        c = sp_cursor(db, SPGT, NULL, 0);
        if (NULL == c) {
          errmsg = sp_error(db);
          return;
        }

        while (sp_fetch(c)) {
          char *key = (char *) sp_key(c);
          if (key) {
            keys[index] = key;
            index++;
          }
        }

        sp_destroy(c);

        if (count != index) {
          // TODO: why?
          errmsg = "Database is corrupt.";
          return;
        }

        for (int i = 0; i < count; i++) {
          size_t keysize = strlen(keys[i]) + 1;
          if (-1 == sp_delete(db, keys[i], keysize)) {
            errmsg = sp_error(db);
            return;
          }
        }
      }

    private:
      void *db;
  };

  void Clear(void *db, NanCallback *callback) {
    ClearWorker *worker = new ClearWorker(db, callback);
    NanAsyncQueueWorker(worker);
  }
}
