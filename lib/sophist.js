
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

Sophist.prototype.get = function (key, cb) {
  key = String(key);
  this.db.get(key, cb);
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
  return new Iterator(this.db, opts || {});
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
