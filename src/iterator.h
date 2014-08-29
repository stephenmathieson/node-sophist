
#ifndef ITERATOR_H
#define ITERATOR_H 1

#include "database.h"

namespace sophist {

class Database;

class Iterator : public node::ObjectWrap {
public:
  static void Init();
  static v8::Local<v8::Object> NewInstance(
      v8::Local<v8::Object> database
    , v8::Local<v8::Number> id
  );

  Iterator(Database *database, uint32_t id);
  ~Iterator();

  Database *database;
  sophia::Iterator *it;
  uint32_t id;

private:
  static NAN_METHOD(New);
  static NAN_METHOD(Next);
  static NAN_METHOD(NextSync);
  static NAN_METHOD(End);
  static NAN_METHOD(EndSync);


protected:
};

} // namespace sophist

#endif
