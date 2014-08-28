
#ifndef ITERATOR_H
#define ITERATOR_H 1

#include "database.h"

namespace sophist {

class Iterator : public node::ObjectWrap {
public:
  static void Init();
  static v8::Local<v8::Object> NewInstance(v8::Local<v8::Object> database);

  Iterator(Database *database);
  ~Iterator();

  Database *database;
  sophia::Iterator *it;

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
