
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
  return this.database.iterator();
};
