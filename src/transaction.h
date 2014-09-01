
#ifndef SOPHIST_TRANSACTION_H
#define SOPHIST_TRANSACTION_H 1

#include "database.h"

namespace sophist {

class Database;

class Transaction : public node::ObjectWrap {
public:
  Transaction(Database *database);
  ~Transaction();
  static void Init();
  static v8::Local<v8::Object> NewInstance(v8::Local<v8::Object> database);

  Database *database;
  sophia::Transaction *t;
  uint32_t id;

private:
  static NAN_METHOD(New);
  static NAN_METHOD(Set);
  static NAN_METHOD(Delete);
  static NAN_METHOD(Commit);
  static NAN_METHOD(Rollback);
};


} // namespace sophist

#endif
