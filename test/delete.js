
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#delete', function () {
  var db;

  before(function () {
    db = new Sophist('./testdb');
  });

  describe('on an unopened database', function () {
    it('should fail', function (done) {
      db.delete('foo', function (err) {
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
        db.delete('abc');
      } catch (e) {
        err = e;
      }
      assert('Callback required' == err.message);
    });

    it('should delete a key/value pair', function (done) {
      db.set('foo', 'bar', function (err) {
        if (err) return done(err);
        db.delete('foo', function (err) {
          if (err) return done(err);
          db.get('foo', function (err, value) {
            if (err) return done(err);
            assert(null === value);
            done();
          });
        });
      });
    });

    it('should handle number coercion', function (done) {
      db.set(123, 456, function (err) {
        if (err) return done(err);
        db.delete(123, function (err) {
          db.get(123, function (err, value) {
            assert(value === null);
            done();
          });
        });
      });
    });

    it('should handle buffer coercion', function (done) {
      var k = new Buffer('buffer key');
      db.set(k, 456, function (err) {
        if (err) return done(err);
        db.delete(k, function (err) {
          db.get(k, function (err, value) {
            assert(value === null);
            done();
          });
        });
      });
    });
  });
});
