
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');

describe('levelup(sophist).open', function () {
  it('should emit "open"', function (done) {
    var db = levelup('./testdb', { db: Sophist });

    db.on('open', function () {
      db.close(done);
    });

    db.open(function (err) {
      if (err) return done(err);
    });
  });

  it('should emit "ready"', function (done) {
    var db = levelup('./testdb', { db: Sophist });

    db.on('ready', function () {
      db.close(done);
    });

    db.open(function (err) {
      if (err) return done(err);
    });
  });
});
