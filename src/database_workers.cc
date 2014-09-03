
#include <nan.h>
#include "sophia-cc.h"
#include "database.h"
#include "database_workers.h"

using namespace sophia;

namespace sophist {

/**
 * Open worker.
 */

OpenWorker::OpenWorker(
    Database *database
  , NanCallback *callback
  , bool create_if_missing
  , bool read_only
  , uint32_t merge_watermark
  , uint32_t page_size
) : DatabaseWorker(database, callback)
  , create_if_missing(create_if_missing)
  , read_only(read_only)
  , merge_watermark(merge_watermark)
  , page_size(page_size) {}

void OpenWorker::Execute() {
  SophiaReturnCode rc = database->sophia->Open(
      create_if_missing
    , read_only
    , page_size
    , merge_watermark
  );
  if (SOPHIA_SUCCESS != rc) {
    SetErrorMessage(database->sophia->Error(rc));
  }
}

/**
 * Close worker.
 */

CloseWorker::CloseWorker(
    Database *database
  , NanCallback *callback
) : DatabaseWorker(database, callback) {}

void CloseWorker::Execute() {
  // cleanup any open iterators
  database->ReleaseIterators();
  SophiaReturnCode rc = database->sophia->Close();
  if (SOPHIA_SUCCESS != rc) {
    SetErrorMessage(database->sophia->Error(rc));
  }
}

/**
 * Set worker.
 */

SetWorker::SetWorker(
    Database *database
  , NanCallback *callback
  , char *key
  , char *value
) : DatabaseWorker(database, callback), key(key), value(value) {}

SetWorker::~SetWorker() {
  delete[] key;
  delete[] value;
}

void SetWorker::Execute() {
  SophiaReturnCode rc = database->sophia->Set(key, value);
  if (SOPHIA_SUCCESS != rc) {
    SetErrorMessage(database->sophia->Error(rc));
  }
}

/**
 * Get worker.
 */

GetWorker::GetWorker(
    Database *database
  , NanCallback *callback
  , char *key
) : DatabaseWorker(database, callback), key(key) {}

GetWorker::~GetWorker() {
  delete[] key;
  if (value) free(value);
}

void GetWorker::Execute() {
  value = database->sophia->Get(key);
}

void GetWorker::HandleOKCallback() {
  NanScope();

  v8::Local<v8::Value> ret;
  if (value) {
    ret = NanNew<v8::String>(value);
  } else {
    ret = NanNull();
  }

  v8::Local<v8::Value> argv[] = {
      NanNull()
    , ret
  };
  callback->Call(2, argv);
}

/**
 * Delete worker.
 */

DeleteWorker::DeleteWorker(
    Database *database
  , NanCallback *callback
  , char *key
) : DatabaseWorker(database, callback), key(key) {}

DeleteWorker::~DeleteWorker() {
  delete[] key;
}

void DeleteWorker::Execute() {
  SophiaReturnCode rc = database->sophia->Delete(key);
  if (SOPHIA_SUCCESS != rc) {
    SetErrorMessage(database->sophia->Error(rc));
  }
}

} // namespace sophist
