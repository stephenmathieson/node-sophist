
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#get', function () {
  var db;

  before(function () {
    db = new Sophist('./testdb');
  });

  describe('on an unopened database', function () {
    it('should fail', function (done) {
      // TODO: this should actually fail, not just provide null
      db.get('foo', function (err, value) {
        assert(null == value);
        done();
      });
    });
  });

  describe('on an open database', function () {
    before(function (done) {
      db.open(done);
    });

    beforeEach(function (done) {
      db.clear(done);
    });

    after(function (done) {
      db.close(done);
    });

    it('should require a callback', function () {
      var err;
      try {
        db.get('abc');
      } catch (e) {
        err = e;
      }
      assert('Callback required' == err.message);
    });

    it('should get a value', function (done) {
      db.set('foo', 'bar', function (err) {
        if (err) return done(err);
        db.get('foo', function (err, value) {
          assert('bar' == value);
          done();
        });
      });
    });

    it('should provide null for non-existant keys', function (done) {
      db.get('blahblah', function (err, value) {
        assert(null == err);
        assert(null == value);
        done();
      });
    });
  });
});
