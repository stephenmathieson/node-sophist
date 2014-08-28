
#ifndef SOPHIST_H
#define SOPHIST_H 1

namespace sophist {

/**
 * Cast the given `str` to a `char *`.
 */

char *StringToCharArray(v8::Local<v8::String> str) {
  int size = str->Utf8Length() + 1;
  char *res = new char[size];
  str->WriteUtf8(res, size);
  return res;
}


} // namespace sophist

#endif
