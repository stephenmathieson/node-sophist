
/**
 * Module dependencies.
 */

var yieldly = require('yieldly');
var binding = require('bindings')('sophist');
var Database = binding.Database;

/**
 * Export `Sophist`.
 */

module.exports = Sophist;

/**
 * Create a new Sophia instance at `path`.
 */

function Sophist(path) {
  if (!(this instanceof Sophist)) {
    return new Sophist(path);
  }
  this.database = new Database(path);
}

/**
 * Delegate methods from C++.  Synchronous methods
 * are direct, while asynchronous methods are
 * delegated through `yieldly`, allowing them to
 * be `yield`'ed.
 */

[
  'open',
  'close',
  'set',
  'get',
  'delete',
].forEach(function (method) {
  Sophist.prototype[method] = yieldly(function () {
    return this.database[method].apply(this.database, arguments);
  });

  Sophist.prototype[method + 'Sync'] = function () {
    return this.database[method + 'Sync'].apply(this.database, arguments);
  };
});

/**
 * Create an iterator.
 */

Sophist.prototype.iterator = function () {
  return new Iterator(this.database);
};

/**
 * Create an iterator on the given `database`.
 *
 * @param {Database} database
 * @api private
 */

function Iterator(database) {
  this.iterator = database.iterator();
}

/**
 * Proxy iterator methods through `yieldly`.
 */

[
  'next',
  'end',
].forEach(function (method) {
  Iterator.prototype[method] = yieldly(function (fn) {
    this.iterator[method](fn);
  });
});
