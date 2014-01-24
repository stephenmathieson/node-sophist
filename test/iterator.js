
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#iterator', function () {
  describe('given an unopen db', function () {
    it('should throw an error', function () {
      var db = new Sophist('./testdb');
      var err = null;
      try {
        db.iterator();
      } catch (e) {
        err = e;
      }
      assert(err);
    });
  });

  it('should create a new iterator', function (done) {
    var db = new Sophist('./testdb');
    db.open(function (err) {
      assert(!err);
      var iterator = db.iterator();
      assert(iterator);
      iterator.end(function (err) {
        if (err) return done(err);
        db.close(done);
      });
    });
  });

  it('should block database writes', function (done) {
    var db = new Sophist('./testdb');
    db.open(function (err) {
      assert(!err);
      var iterator = db.iterator();
      db.set('foo', 'bar', function (err) {
        assert(err);
        iterator.end(function (err) {
          if (err) return done(err);
          db.close(done);
        });
      });
    });
  });

  describe('iterator#next', function () {
    var iterator;
    var db;

    beforeEach(function (done) {
      db = new Sophist('./testdb');
      db.open(function (err) {
        if (err) return done(err);
        db.clear(function (err) {
          if (err) return done(err);
          done();
        });
      });
    });

    afterEach(function (done) {
      iterator.end(function (err) {
        if (err) return done(err);
        db.close(done);
      });
    });

    it('should start at the first key', function (done) {
      db.set('foo', 'bar', function (err) {
        if (err) return done(err);
        iterator = db.iterator();
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert('foo' == key);
          assert('bar' == value);
          done();
        });
      });
    });

    // TODO refactor: pyramid of DOOM
    it('should provide NULLs when it reaches the end', function (done) {
      db.set('baz', 'qax', function (err) {
        if (err) return done(err);

        iterator = db.iterator();
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert('baz' == key);
          assert('qax' == value);

          iterator.next(function (err, key, value) {
            if (err) return done(err);
            assert(null === key);
            assert(null === value);
            done();
          });
        });
      });
    });
  });
});
