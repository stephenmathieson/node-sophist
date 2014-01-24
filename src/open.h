
#ifndef SOPHIST_OPEN_H
#define SOPHIST_OPEN_H 1

#include <node.h>
#include <nan.h>
#include "sophist.h"

namespace sophist {
  void Open(
    Sophist *
  , bool
  , bool
  , bool
  , uint32_t
  , uint32_t
  , NanCallback *
  );
}

#endif
