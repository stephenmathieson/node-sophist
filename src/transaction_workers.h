
#ifndef SOPHIST_TRANSACTION_WORKERS_H
#define SOPHIST_TRANSACTION_WORKERS_H 1

#include <node.h>
#include "sophia-cc.h"
#include "transaction.h"

namespace sophist {

/**
 * Abstract Transaction worker.
 */

class TransactionWorker : public NanAsyncWorker {
public:
  TransactionWorker(
      Transaction *transaction
    , NanCallback *callback
  ) : NanAsyncWorker(callback), transaction(transaction) {}

protected:
  Transaction *transaction;
};

class CommitWorker : public TransactionWorker {
public:
  CommitWorker(
      Transaction *transaction
    , NanCallback *callback
  );
  virtual void Execute();
};

class RollbackWorker : public TransactionWorker {
public:
  RollbackWorker(
      Transaction *transaction
    , NanCallback *callback
  );
  virtual void Execute();
};

} // namespace sophist

#endif
