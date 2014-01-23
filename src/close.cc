
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "sophist.h"
#include "close.h"

namespace sophist {
  class CloseWorker : public NanAsyncWorker {
    public:
      CloseWorker(Sophist *wrapper, NanCallback *callback)
        : NanAsyncWorker(callback)
        , wrapper(wrapper) {}

      void Execute() {
        if (NULL == wrapper->db || NULL == wrapper->env) {
          // TODO this is nasty, but prevents leaks in nan
          errmsg = strdup("Unable to close an unopen database.");

          if (wrapper->db) sp_destroy(wrapper->db);
          if (wrapper->env) sp_destroy(wrapper->env);
          wrapper->db = NULL;
          wrapper->env = NULL;
          return;
        }

        if (-1 == sp_destroy(wrapper->db)) {
          errmsg = sp_error(wrapper->db);
          return;
        }
        wrapper->db = NULL;

        if (-1 == sp_destroy(wrapper->env)) {
          errmsg = sp_error(wrapper->env);
          return;
        }
        wrapper->env = NULL;
      }

    private:
      Sophist *wrapper;
};

  void Close(Sophist *wrapper, NanCallback *callback) {
    CloseWorker *worker = new CloseWorker(wrapper, callback);
    NanAsyncQueueWorker(worker);
  }
}
