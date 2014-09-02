
#include "sophist.h"
#include "iterator.h"
#include "iterator_workers.h"

namespace sophist {

static v8::Persistent<v8::FunctionTemplate> iterator_constructor;

Iterator::Iterator(
    Database *database
  , uint32_t id
  , bool reverse
  , char *start
  , char *end
  , bool gte
) : database(database)
  , id(id)
  , reverse(reverse)
  , start(start)
  , end(end)
  , gte(gte) {}

Iterator::~Iterator() {
  if (start) delete[] start;
  if (end) delete[] end;
}

void Iterator::Init() {
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(
    Iterator::New
  );
  NanAssignPersistent(iterator_constructor, tpl);
  tpl->SetClassName(NanNew("Iterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Iterator::Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "nextSync", Iterator::NextSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "end", Iterator::End);
  NODE_SET_PROTOTYPE_METHOD(tpl, "endSync", Iterator::EndSync);
}

v8::Local<v8::Object> Iterator::NewInstance(
    v8::Local<v8::Object> database
  , v8::Local<v8::Number> id
  , v8::Local<v8::Object> options
) {
  NanEscapableScope();

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> constructor = NanNew<v8::FunctionTemplate>(
    iterator_constructor
  );

  if (options.IsEmpty()) {
    v8::Handle<v8::Value> argv[2] = { database, id };
    instance = constructor->GetFunction()->NewInstance(2, argv);
  } else {
    v8::Handle<v8::Value> argv[3] = { database, id, options };
    instance = constructor->GetFunction()->NewInstance(3, argv);
  }

  return NanEscapeScope(instance);
}

NAN_METHOD(Iterator::New) {
  NanScope();
  Database *database = node::ObjectWrap::Unwrap<Database>(
    args[0]->ToObject()
  );
  v8::Local<v8::Value> id = args[1];
  v8::Local<v8::Object> options;
  bool reverse = false;
  char *start = NULL;
  char *end = NULL;
  bool gte = false;

 if (args.Length() > 1 && args[2]->IsObject()) {
    options = v8::Local<v8::Object>::Cast(args[2]);
    reverse = NanBooleanOptionValue(options, NanNew("reverse"));

    #define STRING_OPTION(name)                                       \
      v8::Local<v8::String> _ ## name = NanNew(#name);                \
      if (options->Has(_ ## name)) {                                  \
        if (!options->Get(_ ## name)->IsString()) {                   \
          return NanThrowError(#name " key must be a string");        \
        }                                                             \
        SP_V8_STRING_TO_CHAR_ARRAY(name, options->Get(_ ## name));    \
      }                                                               \

    STRING_OPTION(start)
    STRING_OPTION(end)

    #undef STRING_OPTION

    gte = NanBooleanOptionValue(options, NanNew("gte"));
  }

  Iterator *iterator = new Iterator(
      database
    , (uint32_t) id->Int32Value()
    , reverse
    , start
    , end
    , gte
  );
  iterator->it = new sophia::Iterator(
      database->sophia
    , reverse ? SPLT : gte ? SPGTE : SPGT
    , start
    , end
  );

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

NAN_METHOD(Iterator::NextSync) {
  NanScope();
  Iterator *iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());
  sophia::IteratorResult *result = iterator->it->Next();

  v8::Local<v8::Array> arr = NanNew<v8::Array>(2);
  arr->Set(0, NanNew(result->key));
  arr->Set(1, NanNew(result->value));
  delete result;
  NanReturnValue(arr);
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

NAN_METHOD(Iterator::EndSync) {
  NanScope();
  Iterator *iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());
  sophia::SophiaReturnCode rc = iterator->it->End();
  if (sophia::SOPHIA_SUCCESS != rc) {
    return NanThrowError(iterator->database->sophia->Error(rc));
  }
  iterator->database->ReleaseIterator(iterator->id);
  NanReturnUndefined();
}

} // namespace sophist
