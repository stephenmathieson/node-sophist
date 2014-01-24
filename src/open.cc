
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "sophist.h"
#include "open.h"

namespace sophist {
  class OpenWorker : public NanAsyncWorker {
    public:
      OpenWorker(
            Sophist *wrapper
          , bool create
          , bool read_only
          , bool gc
          , uint32_t merge_watermark
          , uint32_t page_size
          , NanCallback *callback
        )
        : NanAsyncWorker(callback)
        , wrapper(wrapper)
        , create(create)
        , read_only(read_only)
        , gc(gc)
        , merge_watermark(merge_watermark)
        , page_size(page_size) {}

      void Execute() {
        void *env = NULL;
        void *db = NULL;
        uint32_t flags = 0;

        if (create) flags |= SPO_CREAT;

        if (read_only) {
          flags |= SPO_RDONLY;
        } else {
          flags |= SPO_RDWR;
        }

        if (NULL == (env = sp_env())) {
          // TODO this is nasty, but prevents leaks in nan
          errmsg = strdup("Unable to allocate Sophia environment.");
          return;
        }

        if (-1 == sp_ctl(env, SPDIR, flags, wrapper->path)) {
          errmsg = strdup("Unable to open/create Sophia repository.");
          return;
        }

        if (gc) {
          if (-1 == sp_ctl(env, SPGC, 1)) {
            errmsg = strdup("Unable to enable Sophia garbage collector.");
            return;
          }
        }

        if (-1 == sp_ctl(env, SPMERGEWM, merge_watermark)) {
          errmsg = strdup("Unable to set merge watermark value.");
          return;
        }

        if (-1 == sp_ctl(env, SPPAGE, page_size)) {
          errmsg = strdup("Unable to set max page size value.");
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
      bool create;
      bool read_only;
      bool gc;
      uint32_t merge_watermark;
      uint32_t page_size;
  };

  void Open(
        Sophist *wrapper
      , bool create
      , bool read_only
      , bool gc
      , uint32_t merge_watermark
      , uint32_t page_size
      , NanCallback *callback
    ) {
    OpenWorker *worker = new OpenWorker(wrapper
      , create
      , read_only
      , gc
      , merge_watermark
      , page_size
      , callback);
    NanAsyncQueueWorker(worker);
  }
}
