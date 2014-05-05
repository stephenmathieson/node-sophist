
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#clear', function () {
  var db;

  before(function () {
    db = new Sophist('./testdb');
  });

  describe('on an unopened database', function () {
    it('should fail', function (done) {
      db.clear(function (err) {
        assert(err);
        done();
      });
    });
  });

  describe('on an open database', function () {
    before(function (done) {
      db.open(done);
    });

    after(function (done) {
      db.close(done);
    });

    it('should require a callback', function () {
      var err;
      try {
        db.clear();
      } catch (e) {
        err = e;
      }
      assert('Callback required' == err.message);
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
});
