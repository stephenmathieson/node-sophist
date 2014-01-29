
#include <node.h>
#include <nan.h>
#include <sophia.h>
#include "sophist.h"
#include "transaction.h"
#include "transaction_commit.h"
#include "transaction_rollback.h"

namespace sophist {
  static v8::Persistent<v8::FunctionTemplate> transaction_constructor;

  Transaction::Transaction() {}
  Transaction::~Transaction() {}

  void Transaction::Init () {
    v8::Local<v8::FunctionTemplate> tpl =
      v8::FunctionTemplate::New(Transaction::New);
    NanAssignPersistent(v8::FunctionTemplate, transaction_constructor, tpl);
    tpl->SetClassName(NanSymbol("Transaction"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "commit", Transaction::Commit);
    NODE_SET_PROTOTYPE_METHOD(tpl, "rollback", Transaction::Rollback);
  }

  NAN_METHOD(Transaction::New) {
    NanScope();
    Transaction *transaction = new Transaction();
    Sophist *sp = node::ObjectWrap::Unwrap<Sophist>(args[0]->ToObject());
    if (NULL == sp->db) {
      return NanThrowError("Unable to create a transaction "
                           "on an unopen database.");
    }

    transaction->Wrap(args.This());
    transaction->db = sp->db;

    int rc = sp_begin(sp->db);

    // success
    if (0 == rc) {
      NanReturnValue(args.This());
    }

    // until multiple transactions are supported...
    if (1 == rc) {
      return NanThrowError("Another transaction is active");
    }

    // unknown error
    char *err = sp_error(sp->db);
    if (NULL == err) {
      NanThrowError("Unknown transaction error");
    } else {
      NanThrowError(err);
    }
    NanReturnUndefined();
  }

  NAN_METHOD(Transaction::Commit) {
    NanScope();
    Transaction *t = node::ObjectWrap::Unwrap<Transaction>(args.This());
    v8::Local<v8::Function> cb = args[0].As<v8::Function>();
    sophist::TransactionCommit(t, new NanCallback(cb));
    NanReturnUndefined();
  }

  NAN_METHOD(Transaction::Rollback) {
    NanScope();
    Transaction *t = node::ObjectWrap::Unwrap<Transaction>(args.This());
    v8::Local<v8::Function> cb = args[0].As<v8::Function>();
    sophist::TransactionRollback(t, new NanCallback(cb));
    NanReturnUndefined();
  }

  v8::Local<v8::Object>
  Transaction::NewInstance(v8::Local<v8::Object> sp) {
    NanScope();
    v8::Local<v8::Object> instance;
    v8::Local<v8::FunctionTemplate> constructor =
      NanPersistentToLocal(transaction_constructor);
    v8::Handle<v8::Value> argv[1] = { sp };
    instance = constructor->GetFunction()->NewInstance(1, argv);
    return instance;
  }
}
