
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');

describe('levelup(sophist).close', function () {
  it('should emit "closing"', function (done) {
    var db = levelup('./testdb', { db: Sophist });
    var emitted = false;

    db.on('ready', function () {
      db.on('closing', function () {
        emitted = true;
      });
      db.close(function (err) {
        if (err) throw err;
        assert(emitted);
        done();
      });
    });
  });

  it('should emit "closed"', function (done) {
    var db = levelup('./testdb', { db: Sophist });
    db.open(function (err) {
      if (err) throw err;
      db.on('closed', done);
      db.close();
    });
  });
});
