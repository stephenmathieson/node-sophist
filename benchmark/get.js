
var rimraf = require('rimraf');
var Sophist = require('..');
var LevelDOWN = require('leveldown');

suite('Sophist#get', function () {
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

  bench('get existing random key', function (next) {
    var key = 'key-' + Math.floor(Math.random() * 1000);
    db.get(key, next);
  });

  bench('get missing key', function (next) {
    db.get('abc123', next);
  });

  bench('synchronous get existing random key', function () {
    var key = 'key-' + Math.floor(Math.random() * 1000);
    db.getSync(key);
  });

  bench('synchronous get missing key', function () {
    db.getSync('abc123');
  });
});

suite('LevelDOWN#get', function () {
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

  bench('get existing random key', function (next) {
    var key = 'key-' + Math.floor(Math.random() * 1000);
    db.get(key, next);
  });

  bench('get missing key', function (next) {
    db.get('abc123', next);
  });
});
