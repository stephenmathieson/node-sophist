
var rimraf = require('rimraf');
var Sophist = require('..');
var LevelDOWN = require('leveldown');

suite('Sophist#iterator', function () {
  var db;

  before(function (next) {
    rimraf('./testdb', function (err) {
      if (err) throw err;
      db = new Sophist('./testdb');
      db.open(function (err) {
        if (err) throw err;
        for (var i = 0; i < 1000; i++) {
          db.setSync('key-' + i, 'value-' + i);
        }
        next();
      });
    });
  });

  after(function (next) {
    db.close(next);
  });

  bench('iterate 1000 keys', function (next) {
    var iterator = db.iterator();
    function callback(err, key, value) {
      if (err) throw err;
      if (undefined === key) return iterator.end(next);
      iterator.next(callback);
    }
    iterator.next(callback);
  });
});

suite('LevelDOWN#iterator', function () {
  var db;

  before(function (done) {
    rimraf('./testdb', function (err) {
      if (err) throw err;
      db = new LevelDOWN('./testdb');
      db.open(function (err) {
        if (err) throw err;
        var i = -1;
        (function next(err) {
          if (err) throw err;
          if (1000 == ++i) return done();
          db.put('key-' + i, 'value-' + i, next);
        }());
      });
    });
  });

  after(function (next) {
    db.close(next);
  });

  bench('iterate 1000 keys', function (next) {
    var iterator = db.iterator();
    function callback(err, key, value) {
      if (err) throw err;
      if (undefined === key) return iterator.end(next);
      iterator.next(callback);
    }
    iterator.next(callback);
  });
});
