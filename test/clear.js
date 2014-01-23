
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#clear', function () {
  var db;

  before(function (done) {
    db = new Sophist('./testdb');
    db.open(done);
  });

  after(function (done) {
    db.close(done);
  });

  it('should clear all keys', function (done) {
    db.clear(function (err) {
      if (err) throw err;
      db.count(function (err, count) {
        assert(null == err);
        assert(0 == count);
        done();
      });
    });
  });
});
