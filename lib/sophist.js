
var fs = require('fs');
var path = require('path');
var DB = require('bindings')('sophist').Sophist;

module.exports = Sophist;

function Sophist(path) {
  this.path = path;
  this.db = new DB(path);
}

Sophist.prototype.open = function (opts, cb) {
  if ('function' == typeof opts) {
    cb = opts;
    opts = {};
  }
  this.db.open(opts, cb);
};

Sophist.prototype.close = function (cb) {
  this.db.close(cb);
};

Sophist.prototype.set = function (key, value, cb) {
  key = String(key);
  value = String(value);
  this.db.set(key, value, cb);
};

Sophist.prototype.setSync = function (key, value) {
  key = String(key);
  value = String(value);
  this.db.setSync(key, value);
};

Sophist.prototype.get = function (key, cb) {
  key = String(key);
  this.db.get(key, cb);
};

Sophist.prototype.getSync = function (key) {
  key = String(key);
  return this.db.getSync(key);
};

Sophist.prototype.delete = function (key, cb) {
  key = String(key);
  this.db.delete(key, cb);
};

Sophist.prototype.count = function (cb) {
  this.db.count(cb);
};

Sophist.prototype.clear = function (cb) {
  this.db.clear(cb);
};

Sophist.prototype.purge = function (cb) {
  var self = this;
  fs.readdir(this.path, function (err, files) {
    if (err) return cb(err);

    var list = files.filter(function (file) {
      return '.log' != file.substr(file.length - 4);
    });

    var pending = list.length;
    var done = false;

    // TODO may cause EMFILEs
    list.forEach(function (file) {
      file = path.join(self.path, file);
      fs.unlink(file, function (err) {
        if (!done && (err || !--pending)) {
          done = true;
          cb(err);
        }
      });
    });
  });
};

Sophist.prototype.iterator = function (opts) {
  opts = opts || {};

  if (opts.start && 'string' != typeof opts.start) {
    throw new TypeError('Start must be a string');
  }

  if (opts.end && 'string' != typeof opts.end) {
    throw new TypeError('End must be a string');
  }

  return new Iterator(this.db, opts);
};

Sophist.prototype.transaction = function () {
  return new Transaction(this.db);
};

function Iterator(db, opts) {
  this.db = db;
  this.it = this.db.iterator(opts);
}

Iterator.prototype.next = function (cb) {
  this.it.next(cb);
};

Iterator.prototype.end = function (cb) {
  this.it.end(cb);
};

function Transaction(db) {
  this.db = db;
  this.operations = [];
  this.t = this.db.transaction();
}

Transaction.prototype.commit = function (cb) {
  if (!this.operations.length) return this.t.commit(cb);

  var self = this;
  var index = -1;

  (function next(err) {
    if (err) return cb(err);

    var op = self.operations[++index];
    if (!op) return self.t.commit(cb);
    if ('set' == op.type) {
      self.db.set(op.key, op.value, next);
    } else {
      self.db.delete(op.key, next);
    }
  }());
};

Transaction.prototype.rollback = function (cb) {
  // TODO empty operations array?
  this.t.rollback(cb);
};

Transaction.prototype.set = function (key, value) {
  this.operations.push({
    type: 'set',
    key: key,
    value: value
  });
  return this;
};

Transaction.prototype.delete = function (key) {
  this.operations.push({
    type: 'delete',
    key: key
  });
  return this;
};
