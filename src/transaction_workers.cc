
#include <nan.h>
#include "sophia-cc.h"
#include "transaction_workers.h"

using namespace sophia;

namespace sophist {

CommitWorker::CommitWorker(
    Transaction *transaction
  , NanCallback *callback
) : TransactionWorker(transaction, callback) {}

void CommitWorker::Execute() {
  SophiaReturnCode rc = transaction->t->Commit();
  transaction->database->transaction = NULL;
  if (SOPHIA_SUCCESS != rc) {
    SetErrorMessage(transaction->database->sophia->Error(rc));
  }
}

RollbackWorker::RollbackWorker(
    Transaction *transaction
  , NanCallback *callback
) : TransactionWorker(transaction, callback) {}

void RollbackWorker::Execute() {
  SophiaReturnCode rc = transaction->t->Rollback();
  transaction->database->transaction = NULL;
  if (SOPHIA_SUCCESS != rc) {
    SetErrorMessage(transaction->database->sophia->Error(rc));
  }
}

} // namespace sophist
