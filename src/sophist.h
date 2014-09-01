
#ifndef SOPHIST_H
#define SOPHIST_H 1

namespace sophist {

#define SP_V8_STRING_TO_CHAR_ARRAY(name, from)                       \
  size_t name ## _size;                                              \
  v8::Local<v8::String> name ## _str = from->ToString();             \
  name ## _size = name ## _str->Utf8Length() + 1;                    \
  name = new char[name ## _size];                                    \
  name ## _str->WriteUtf8(                                           \
      name                                                           \
    , name ## _size                                                  \
  );

} // namespace sophist

#endif
