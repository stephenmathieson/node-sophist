
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "transaction.h"
#include "transaction_rollback.h"

namespace sophist {
  class TransactionRollbackWorker : public NanAsyncWorker {
    public:
      TransactionRollbackWorker(Transaction *t, NanCallback *callback)
        : NanAsyncWorker(callback) {
          db = t->db;
        }

      void Execute() {
        if (-1 == sp_rollback(db)) errmsg = strdup(sp_error(db));
      }

    private:
      void *db;
  };

  void TransactionRollback(Transaction *t, NanCallback *callback) {
    TransactionRollbackWorker *worker =
      new TransactionRollbackWorker(t, callback);
    NanAsyncQueueWorker(worker);
  }
}
