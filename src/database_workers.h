
#ifndef DATABASE_WORKERS_H
#define DATABASE_WORKERS_H 1

#include <node.h>

namespace sophist {

/**
 * Abstract Database worker.
 */

class DatabaseWorker : public NanAsyncWorker {
public:
  DatabaseWorker(
      Database *database
    , NanCallback *callback
  ) : NanAsyncWorker(callback), database(database) {}

protected:
  Database *database;
};

class OpenWorker : public DatabaseWorker {
public:
  OpenWorker(
      Database *database
    , NanCallback *callback
  );

  virtual void Execute();
};

class CloseWorker : public DatabaseWorker {
public:
  CloseWorker(
      Database *database
    , NanCallback *callback
  );

  virtual void Execute();
};

class SetWorker : public DatabaseWorker {
public:
  SetWorker(
      Database *database
    , NanCallback *callback
    , char *key
    , char *value
  );

  virtual ~SetWorker();
  virtual void Execute();

private:
  char *key;
  char *value;
};

class GetWorker : public DatabaseWorker {
public:
  GetWorker(
      Database *database
    , NanCallback *callback
    , char *key
  );

  virtual ~GetWorker();
  virtual void HandleOKCallback();
  virtual void Execute();

private:
  char *key;
  char *value;
};


class DeleteWorker : public DatabaseWorker {
public:
  DeleteWorker(
      Database *database
    , NanCallback *callback
    , char *key
  );

  virtual ~DeleteWorker();
  virtual void Execute();

private:
  char *key;
};

}; // namespace sophist

#endif
