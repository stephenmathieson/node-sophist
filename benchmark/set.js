
var rimraf = require('rimraf');
var Sophist = require('..');
var LevelDOWN = require('leveldown');

var key100 = Array(100).join('k');
var value100 = Array(100).join('v');
var key1000 = Array(1000).join('k');
var value1000 = Array(1000).join('v');

suite('Sophist#set', function () {
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

  bench('set 1 char key/value', function (next) {
    db.set('k', 'v', next);
  });

  bench('set 100 char key/value', function (next) {
    db.set(key100, value100, next);
  });

  bench('set 1000 char key/value', function (next) {
    db.set(key1000, value1000, next);
  });

  bench('synchronous set 1 char key/value', function () {
    db.setSync('k', 'v');
  });

  bench('synchronous set 100 char key/value', function () {
    db.setSync(key100, value100);
  });

  bench('synchronous set 1000 char key/value', function () {
    db.setSync(key1000, value1000);
  });
});

suite('LevelDOWN#put', function () {
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

  bench('put 1 char key/value', function (next) {
    db.put('k', 'v', next);
  });

  bench('put 100 char key/value', function (next) {
    db.put(key100, value100, next);
  });

  bench('put 1000 char key/value', function (next) {
    db.put(key1000, value1000, next);
  });
});
