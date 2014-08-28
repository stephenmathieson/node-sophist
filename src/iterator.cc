
#include "iterator.h"
#include "iterator_workers.h"

namespace sophist {

static v8::Persistent<v8::FunctionTemplate> iterator_constructor;

Iterator::Iterator(Database *database) : database(database) {}

Iterator::~Iterator() {}

void Iterator::Init() {
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(
    Iterator::New
  );
  NanAssignPersistent(iterator_constructor, tpl);
  tpl->SetClassName(NanNew("Iterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Iterator::Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "end", Iterator::End);
}

v8::Local<v8::Object> Iterator::NewInstance(
  v8::Local<v8::Object> database
) {
  NanEscapableScope();

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> c = NanNew<v8::FunctionTemplate>(
    iterator_constructor
  );

  // TODO: options, etc
  v8::Handle<v8::Value> argv[1] = { database };
  instance = c->GetFunction()->NewInstance(1, argv);

  return NanEscapeScope(instance);
}

NAN_METHOD(Iterator::New) {
  NanScope();
  Database *database = node::ObjectWrap::Unwrap<Database>(
    args[0]->ToObject()
  );
  Iterator *iterator = new Iterator(database);
  iterator->it = new sophia::Iterator(database->sophia);

  sophia::SophiaReturnCode rc = iterator->it->Begin();
  if (sophia::SOPHIA_SUCCESS != rc) {
    NanThrowError(database->sophia->Error(rc));
  }

  iterator->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(Iterator::Next) {
  NanScope();

  if (0 == args.Length() || !args[0]->IsFunction()) {
    NanThrowError("callback required");
    NanReturnUndefined();
  }

  Iterator *iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());
  v8::Local<v8::Function> callback = args[0].As<v8::Function>();
  NextWorker *worker = new NextWorker(
      iterator
    , new NanCallback(callback)
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("iterator", _this);
  NanAsyncQueueWorker(worker);

  NanReturnValue(args.Holder());
}

NAN_METHOD(Iterator::End) {
  NanScope();

  if (0 == args.Length() || !args[0]->IsFunction()) {
    NanThrowError("callback required");
    NanReturnUndefined();
  }

  Iterator *iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());
  v8::Local<v8::Function> callback = args[0].As<v8::Function>();
  EndWorker *worker = new EndWorker(
      iterator
    , new NanCallback(callback)
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("iterator", _this);
  NanAsyncQueueWorker(worker);

  NanReturnValue(args.Holder());
}

} // namespace sophist
