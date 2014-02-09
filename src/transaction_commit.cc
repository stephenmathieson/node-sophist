
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
          operations = t->operations;
        }

      void Execute() {
        list_node_t *node;
        list_iterator_t *it = list_iterator_new(operations, LIST_HEAD);
        while ((node = list_iterator_next(it))) {
          Operation *operation = (Operation *) node->val;
          int rc;

          if (OPERATION_SET == operation->type) {
            size_t keysize = strlen(operation->key) + 1;
            size_t valuesize = strlen(operation->value) + 1;
            rc = sp_set(db
                , operation->key
                , keysize
                , operation->value
                , valuesize);
          } else {
            size_t keysize = strlen(operation->key) + 1;
            rc = sp_delete(db
                , operation->key
                , keysize);
          }

          if (-1 == rc) {
            list_iterator_destroy(it);
            errmsg = strdup(sp_error(db));
            return;
          }
        }

        list_iterator_destroy(it);

        if (-1 == sp_commit(db))
          errmsg = strdup(sp_error(db));
      }

    private:
      void *db;
      list_t *operations;
  };

  void TransactionCommit(Transaction *t, NanCallback *callback) {
    TransactionCommitWorker *worker =
      new TransactionCommitWorker(t, callback);
    NanAsyncQueueWorker(worker);
  }
}
