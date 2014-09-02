
/**
 * Module dependencies.
 */

var binding = require('bindings')('sophist');
var Database = binding.Database;
var yieldly = require('yieldly');

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
 * delegated through `yieldly`.
 */

[
  'open',
  'close',
  'set',
  'get',
  'delete',
].forEach(function (method) {
  var sync = method + 'Sync';
  Sophist.prototype[method] = yieldly(function () {
    return this.database[method].apply(this.database, arguments);
  });

  Sophist.prototype[sync] = function () {
    return this.database[sync].apply(this.database, arguments);
  };
});

/**
 * Create an iterator.
 */

Sophist.prototype.iterator = function (options) {
  return new Iterator(this.database, options);
};

/**
 * Create a transaction.
 */

Sophist.prototype.transaction = function () {
  return new Transaction(this.database);
};

/**
 * Create an iterator on the given `database`.
 *
 * @param {Database} database
 * @api private
 */

function Iterator(database, options) {
  this.iterator = database.iterator(options);
}

/**
 * Proxy synchronous methods directly, async
 * methods through `yieldly`.
 */

[
  'next',
  'end',
].forEach(function (method) {
  var sync = method + 'Sync';

  Iterator.prototype[method] = yieldly(function (fn) {
    this.iterator[method](fn);
  });

  Iterator.prototype[sync] = function () {
    return this.iterator[sync]();
  };
});

/**
 * Create an transaction on the given `database`.
 *
 * @param {Database} database
 * @api private
 */

function Transaction(database) {
  this.transaction = database.transaction();
}

/**
 * Proxy write methods to C++.
 */

[
  'set',
  'delete',
].forEach(function (method) {
  Transaction.prototype[method] = function () {
    this.transaction[method].apply(this.transaction, arguments);
    return this;
  };
});

/**
 * Proxy transaction methods through `yieldly`.
 */

[
  'commit',
  'rollback',
].forEach(function (method) {
  Transaction.prototype[method] = yieldly(function (fn) {
    this.transaction[method](fn);
  });
});
