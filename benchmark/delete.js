
var rimraf = require('rimraf');
var Sophist = require('..');
var LevelDOWN = require('leveldown');

var key100 = Array(100).join('k');
var key1000 = Array(1000).join('k');

suite('Sophist#delete', function () {
  var db;

  before(function (next) {
    rimraf('./testdb', function (err) {
      if (err) throw err;
      db = new Sophist('./testdb');
      db.open(next);
    });
  });

  after(function (next) {
    db.close(next);
  });

  bench('delete 1 char key/value', function (next) {
    db.delete('k', next);
  });

  bench('delete 100 char key', function (next) {
    db.delete(key100, next);
  });

  bench('delete 1000 char key', function (next) {
    db.delete(key1000, next);
  });
});

suite('LevelDOWN#del', function () {
  var db;

  before(function (next) {
    rimraf('./testdb', function (err) {
      if (err) throw err;
      db = new LevelDOWN('./testdb');
      db.open(next);
    });
  });

  after(function (next) {
    db.close(next);
  });

  bench('del 1 char key', function (next) {
    db.del('k', next);
  });

  bench('del 100 char key', function (next) {
    db.del(key100, next);
  });

  bench('del 1000 char key', function (next) {
    db.del(key1000, next);
  });
});
