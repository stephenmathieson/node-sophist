
var fs = require('fs');
var path = require('path');
var DB = require('bindings')('sophist').Sophist;

module.exports = Sophist;

/**
 * Initialize a Sophia binding for `path`.
 *
 * @api public
 * @param {String} path
 */

function Sophist(path) {
  if (!(this instanceof Sophist)) return new Sophist(path);
  this.path = path;
  this.db = new DB(path);
}

/**
 * Open/create the Sophia database with the
 * given `options`, invoking `cb(err)`.
 *
 * Options:
 *
 *  - `createIfMissing` - boolean, default `true`
 *  - `readOnly` - boolean, default `false`
 *  - `gc` - boolean, default `true`
 *  - `pageSize` - number, default `2048`
 *  - `mergeWatermark` - number, default `100000`
 *
 * @api public
 * @param {Object} [options]
 * @param {Function} cb
 */

Sophist.prototype.open = function (options, cb) {
  if ('function' == typeof options) {
    cb = options;
    options = {};
  }
  this.db.open(options, cb);
};

/**
 * Close the Sophia database, invoking `cb(err)`.
 *
 * @api public
 * @param {Function} cb
 */

Sophist.prototype.close = function (cb) {
  this.db.close(cb);
};

/**
 * Set `key=value` in the database, invoking `cb(err)`.
 *
 * @api public
 * @param {String|Buffer|Number} key
 * @param {String|Buffer|Number} value
 * @param {Function} cb
 */

Sophist.prototype.set = function (key, value, cb) {
  key = String(key);
  value = String(value);
  this.db.set(key, value, cb);
};

/**
 * LevelUP compatible `put`.
 *
 * @api public
 * @param {String|Buffer} key
 * @param {String|Buffer} value
 * @param {Object} [options] Unused
 * @param {Function} cb
 */

Sophist.prototype.put = function (key, value, options, cb) {
  cb = 'function' == typeof options
    ? options
    : cb;
  this.set(key, value, cb);
};

/**
 * Synchronously set `key=value`.
 *
 * @api public
 * @param {String|Buffer|Number} key
 * @param {String|Buffer|Number} value
 */

Sophist.prototype.setSync = function (key, value) {
  key = String(key);
  value = String(value);
  this.db.setSync(key, value);
};

/**
 * Get the value of `key`, invoking `cb(err, value)`.
 *
 * @api public
 * @param {String|Buffer|Number} key
 * @param {Object} options Unused
 * @param {Function} cb
 */

Sophist.prototype.get = function (key, options, cb) {
  if (!key) {
    throw new Error('key cannot be `null` or `undefined`');
  }

  key = String(key);
  // levelup support
  cb = 'function' == typeof options
    ? options
    : cb;
  this.db.get(key, cb);
};

/**
 * Synchronously get `key`.
 *
 * @api public
 * @param {String|Buffer|Number} key
 * @return {String}
 */

Sophist.prototype.getSync = function (key) {
  key = String(key);
  return this.db.getSync(key);
};

/**
 * Delete `key` from the database, invoking `cb(err)`.
 *
 * @api public
 * @param {String|Buffer|Number} key
 * @param {Function} cb
 */

Sophist.prototype.delete = function (key, cb) {
  key = String(key);
  this.db.delete(key, cb);
};


/**
 * Synchronously delete `key`.
 *
 * @api public
 * @param {String|Buffer|Number} key
 */

Sophist.prototype.deleteSync = function (key) {
  key = String(key);
  this.db.deleteSync(key);
};

/**
 * LevelUP compatible `del`.
 *
 * @api public
 * @param {String|Buffer} key
 * @param {Object} [options] Unused
 * @param {Function} cb
 */

Sophist.prototype.del = function (key, options, cb) {
  cb = 'function' == typeof options
    ? options
    : cb;
  this.delete(key, cb);
};

/**
 * Get the number of keys in the database, invoking `cb(err, num)`.
 *
 * @api public
 * @param {Function} cb
 */

Sophist.prototype.count = function (cb) {
  this.db.count(cb);
};

/**
 * Clear all keys from the database, invoking `cb(err)`.
 *
 * @api public
 * @param {Function} cb
 */

Sophist.prototype.clear = function (cb) {
  this.db.clear(cb);
};

/**
 * Purge (delete) all incomplete / temporary data
 * from the database.  This operation is **NOT**
 * recoverable.
 *
 * @api public
 * @param {Function} cb
 */

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

/**
 * Create an `Iterator` for the database.
 *
 * You may **not** write data while an iterator is open.
 *
 * Options:
 *
 *  - `reverse` - boolean, default `false`
 *  - `start` - string, default `null`
 *  - `end` - string, default `null`
 *  - `gte` - boolean, default `false`
 *
 * @api public
 * @param {Object} opts
 */

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

/**
 * Create a `Transaction` for `db`.
 */

Sophist.prototype.transaction = function () {
  return new Transaction(this.db);
};

/**
 * LevelUP compatible `batch`.
 *
 * @api public
 * @param {Array} operations
 * @param {Object} [options]
 * @param {Function} callback
 */

Sophist.prototype.batch = function (operations, options, callback) {
  if (!arguments.length) return this.transaction();
  var batch = this.transaction(options);

  for (var i = 0, len = operations.length; i < len; i++) {
    var operation = operations[i];
    if ('set' == operation.type || 'put' == operation.type) {
      batch.set(operation.key, operation.value);
    } else {
      batch.delete(operation.key);
    }
  }

  batch.commit(callback);
};

/**
 * Create an `Iterator` for the given `db` with `opts`.
 *
 * @api private
 * @param {SophistBinding} db
 * @param {Object} [opts]
 */

function Iterator(db, opts) {
  this.db = db;
  this.it = this.db.iterator(opts);
}

/**
 * Get the next `key/value` pair from the database,
 * invoking `cb(err, key, value)`.
 *
 * When the iterator has cycled through all data,
 * will call `cb(null, null, null)`.
 *
 * @api public
 * @param {Function} cb
 */

Iterator.prototype.next = function (cb) {
  this.it.next(cb);
};

/**
 * End the iteration cycle, invoking `cb(err)`.
 *
 * @api public
 * @param {Function} cb
 */

Iterator.prototype.end = function (cb) {
  this.it.end(cb);
};

/**
 * Create a `Transaction` for `db`.
 *
 * @api private
 * @param {SophistBinding} db
 */

function Transaction(db) {
  this.db = db;
  this.t = this.db.transaction();
}

/**
 * Commit the transaction operations, invoking `cb(err)`.
 *
 * @api public
 * @param {Function} cb
 */

Transaction.prototype.write =
Transaction.prototype.commit = function (cb) {
  this.t.commit(cb);
};

/**
 * Disregard the transaction operations, invoking `cb(err)`.
 *
 * Committed operations may not be rolled back.
 *
 * @api public
 * @param {Function} cb
 */

Transaction.prototype.rollback = function (cb) {
  // TODO empty operations array?
  this.t.rollback(cb);
};

/**
 * Create a pending `set` operation.  Operation will
 * not be written until `#commit()` is called.
 *
 * @api public
 * @param {String|Buffer|Number} key
 * @param {String|Buffer|Number} value
 * @return {Transaction} for chaining
 */

Transaction.prototype.put =
Transaction.prototype.set = function (key, value) {
  key = String(key);
  value = String(value);
  this.t.set(key, value);
  return this;
};

/**
 * Create a pending `delete` operation.  Operation
 * will not be written until `#commit()` is called.
 *
 * @api public
 * @param {String|Buffer|Number} key
 * @return {Transaction} for chaining
 */

Transaction.prototype.del =
Transaction.prototype.delete = function (key) {
  key = String(key);
  this.t.delete(key);
  return this;
};
