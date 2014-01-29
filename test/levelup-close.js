
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');

describe('levelup(sophist).close', function () {
  it('should emit "closing"', function (done) {
    var db = levelup('./testdb', { db: Sophist });

    db.on('ready', function () {
      db.on('closing', done);
      db.close();
    });
  });

  it('should emit "closed"', function (done) {
    var db = levelup('./testdb', { db: Sophist });

    db.on('ready', function () {
      db.on('closed', done);
      db.close();
    });
  });
});
