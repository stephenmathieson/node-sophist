
var DB = require('bindings')('sophist').Sophist;

module.exports = Sophist;

function Sophist(path) {
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
