
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#count', function () {
  var db;

  before(function () {
    db = new Sophist('./testdb');
  });

  describe('on an unopened database', function () {
    it('should fail', function (done) {
      db.count(function (err) {
        assert(err);
        done();
      });
    });
  });

  describe('on an open database', function () {
    before(function (done) {
      db.open(done);
    });

    before(function (done) {
      db.clear(done);
    });

    after(function (done) {
      db.close(done);
    });

    it('should require a callback', function () {
      var err;
      try {
        db.count();
      } catch (e) {
        err = e;
      }
      assert('Callback required' == err.message);
    });

    it('should return the number of keys in the db', function (done) {
      db.set('foo', 'bar', function (err) {
        if (err) throw err;
        db.count(function (err, count) {
          assert(1 == count);
          done();
        });
      });
    });
  });
});
