
var Sophist = require('..');
var assert = require('better-assert');
var rimraf = require('rimraf');
var exists = require('fs').existsSync;

describe('Sophist#open', function() {
  describe('creating a database', function () {
    beforeEach(function (done) {
      rimraf('./testdb', done);
    });

    it('should create a folder', function (done) {
      var db = new Sophist('./testdb');
      db.open(function (err) {
        if (err) return done(err);
        assert(exists('./testdb'));
        db.close(done);
      });
    });

    it('should allow reading and writing (by default)', function (done) {
      var db = new Sophist('./testdb');
      db.open(function (err) {
        if (err) return done(err);
        db.set('key', 'value', function (err) {
          if (err) return done(err);
          db.get('key', function (err, value) {
            assert('value' == value);
            db.close(done);
          });
        });
      });
    });

    it('should fail if createIfMissing == false', function (done) {
      var db = new Sophist('./testdb');
      db.open({ createIfMissing: false }, function (err) {
        assert(err);
        done();
      });
    });
  });

  describe('opening an existing database', function () {
    before(function (done) {
      var db = new Sophist('./testdb');
      db.open(function (err) {
        if (err) return done(err);
        db.close(done);
      });
    });

    it('should allow reading and writing (by default)', function (done) {
      var db = new Sophist('./testdb');
      db.open(function (err) {
        if (err) return done(err);
        db.set('key', 'value', function (err) {
          if (err) return done(err);
          db.get('key', function (err, value) {
            assert('value' == value);
            db.close(done);
          });
        });
      });
    });
  });

  describe('opening an open database', function () {
    it('should cause handleable errors', function (done) {
      var db = new Sophist('./testdb');
      db.open(function (err) {
        if (err) return done(err);
        var db2 = new Sophist('./testdb');
        db.open(function (err) {
          assert(err);
          db.close(done);
        });
      });
    });
  });
});
