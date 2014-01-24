
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#set', function () {
  var db;

  before(function (done) {
    db = new Sophist('./testdb');
    db.open(done);
  });

  after(function (done) {
    db.close(done);
  });

  it('should set key=value', function (done) {
    db.set('foo', 'bar', function (err) {
      if (err) return done(err);
      db.get('foo', function (err, value) {
        if (err) return done(err);
        assert('bar' == value);
        done();
      });
    });
  });

  it('should set similar keys', function (done) {
    db.set('key0', 'value0', function (err) {
      if (err) return done(err);
      db.set('key00', 'value00', function (err) {
        if (err) return done(err);
        db.set('key000', 'value000', function (err) {
          if (err) return done(err);
          db.get('key00', function (err, value) {
            if (err) return done(err);
            assert('value00' == value);
            done();
          });
        });
      });
    });
  });

  it('should handle number and buffer coercion', function (done) {
    db.set(12345, 67890, function (err) {
      if (err) return done(err);
      db.get(12345, function (err, value) {
        assert('67890' === value);
        var k = new Buffer('buffer');
        var v = new Buffer('buffers are fun');
        db.set(k, v, function (err) {
          if (err) return done(err);
          db.get('buffer', function (err, value) {
            assert('buffers are fun' === value);
            done();
          });
        });
      });
    });
  });

  it('should fail if readOnly == true', function (done) {
    db.close(function (err) {
      if (err) return done(err);
      db.open({ readOnly: true }, function (err) {
        if (err) return done(err);
        db.set('abc', 'def', function (err) {
          assert(err);
          done();
        });
      });
    });
  });
});
