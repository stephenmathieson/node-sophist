
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "transaction.h"
#include "transaction_commit.h"

namespace sophist {
  class TransactionCommitWorker : public NanAsyncWorker {
    public:
      TransactionCommitWorker(Transaction *t, NanCallback *callback)
        : NanAsyncWorker(callback) {
          db = t->db;
        }

      void Execute() {
        if (-1 == sp_commit(db)) errmsg = strdup(sp_error(db));
      }

    private:
      void *db;
  };

  void TransactionCommit(Transaction *t, NanCallback *callback) {
    TransactionCommitWorker *worker =
      new TransactionCommitWorker(t, callback);
    NanAsyncQueueWorker(worker);
  }
}
