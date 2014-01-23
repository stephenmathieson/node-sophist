
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "sophist.h"
#include "open.h"

namespace sophist {
  class OpenWorker : public NanAsyncWorker {
    public:
      OpenWorker(Sophist *wrapper, NanCallback *callback)
        : NanAsyncWorker(callback)
        , wrapper(wrapper) {}

      void Execute() {
        int rc = 0;
        void *env = NULL;
        void *db = NULL;

        if (NULL == (env = sp_env())) {
          // TODO this is nasty, but prevents leaks in nan
          errmsg = strdup("Unable to allocate Sophia environment.");
          return;
        }

        // TODO flags
        rc = sp_ctl(env, SPDIR, SPO_CREAT|SPO_RDWR, wrapper->path);
        if (-1 == rc) {
          errmsg = strdup("Unable to open/create Sophia repository.");
          return;
        }

        rc = sp_ctl(env, SPGC, 1);
        if (-1 == rc) {
          errmsg = strdup("Unable to enabled Sophia garbage collector.");
          return;
        }

        db = sp_open(env);
        if (NULL == db) {
          // hack.  err is destroyed when env is destroyed
          errmsg = strdup(sp_error(env));
          sp_destroy(env);
          return;
        }

        wrapper->db = db;
        wrapper->env = env;
      }

    private:
      Sophist *wrapper;
  };

  void Open(Sophist *wrapper, NanCallback *callback) {
    OpenWorker *worker = new OpenWorker(wrapper, callback);
    NanAsyncQueueWorker(worker);
  }
}
