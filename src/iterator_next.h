
#ifndef SOPHIST_ITERATOR_NEXT_H
#define SOPHIST_ITERATOR_NEXT_H 1

#include <node.h>
#include <nan.h>
#include "iterator.h"

namespace sophist {
  void IteratorNext(Iterator *, NanCallback *);
}

#endif
