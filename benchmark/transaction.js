
var rimraf = require('rimraf');
var Sophist = require('..');
var LevelDOWN = require('leveldown');

suite('Sophist#transaction', function () {
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

  bench('100 sets', function (next) {
    var t = db.transaction();
    for (var i = 0; i < 100; i++) {
      t.set('key-' + i, 'value-' + i);
    }
    t.commit(next);
  });

  bench('100 deletes', function (next) {
    var t = db.transaction();
    for (var i = 0; i < 100; i++) {
      t.delete('key-' + i);
    }
    t.commit(next);
  });

  bench('50 sets, 50 deletes', function (next) {
    var t = db.transaction();
    for (var i = 0; i < 50; i++) {
      t.set('key-' + i, 'value-' + i);
      t.delete('key-' + i);
    }
    t.commit(next);
  });
});

suite('LevelDOWN#batch', function () {
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

  bench('100 puts', function (next) {
    var b = db.batch();
    for (var i = 0; i < 100; i++) {
      b.put('key-' + i, 'value-' + i);
    }
    b.write(next);
  });

  bench('100 dels', function (next) {
    var b = db.batch();
    for (var i = 0; i < 100; i++) {
      b.del('key-' + i);
    }
    b.write(next);
  });

  bench('50 puts, 50 dels', function (next) {
    var t = db.batch();
    for (var i = 0; i < 50; i++) {
      t.put('key-' + i, 'value-' + i);
      t.del('key-' + i);
    }
    t.write(next);
  });
});
