
#ifndef SOPHIST_GET_H
#define SOPHIST_GET_H 1

#include <node.h>
#include <nan.h>

namespace sophist {
  void Get(void *, char *, NanCallback *);
  char *GetSync(void *, char *);
}

#endif
