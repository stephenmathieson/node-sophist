
#ifndef ITERATOR_WORKERS_H
#define ITERATOR_WORKERS_H 1

#include <node.h>
#include "sophia-cc.h"
#include "iterator.h"

namespace sophist {

/**
 * Abstract Iterator worker.
 */

class IteratorWorker : public NanAsyncWorker {
public:
  IteratorWorker(
      Iterator *iterator
    , NanCallback *callback
  ) : NanAsyncWorker(callback), iterator(iterator) {}

protected:
  Iterator *iterator;
};

class NextWorker : public IteratorWorker {
public:
  NextWorker(
      Iterator *iterator
    , NanCallback *callback
  );

  virtual void Execute();
  virtual void HandleOKCallback();

private:
  sophia::IteratorResult *result;
};

class EndWorker : public IteratorWorker {
public:
  EndWorker(
      Iterator *iterator
    , NanCallback *callback
  );

  virtual void Execute();
  virtual void HandleOKCallback();
};

} // namespace sophist

#endif
