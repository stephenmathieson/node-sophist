
var assert = require('better-assert');
var Sophist = require('..');

function setup(db, fn) {
  db.open(function (err) {
    if (err) throw err;
    db.clear(function (err) {
      if (err) throw err;
      db.set('abc', 123, function (err) {
        if (err) throw err;
        db.set('def', 456, function (err) {
          if (err) throw err;
          db.set('ghi', 789, function (err) {
            if (err) throw err;
            fn();
          });
        });
      });
    });
  });
}

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

    it('should start at the first key (by default)', function (done) {
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
    it('should provide `undefined` when it reaches the end', function (done) {
      db.set('baz', 'qax', function (err) {
        if (err) return done(err);

        iterator = db.iterator();
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert('baz' == key);
          assert('qax' == value);

          iterator.next(function (err, key, value) {
            if (err) return done(err);
            assert(undefined === key);
            assert(undefined === value);
            done();
          });
        });
      });
    });
  });

  describe('given reverse == true', function () {
    var db;

    before(function (done) {
      db = new Sophist('./testdb');
      setup(db, done);
    });

    after(function (done) {
      db.close(done);
    });

    it('should iterate in reverse', function (done) {
      var iterator = db.iterator({ reverse: true });
      iterator.next(function (err, key, value) {
        if (err) return done(err);
        assert('ghi' == key);
        assert(789 == value);
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert('def' == key);
          assert(456 == value);
          iterator.next(function (err, key, value) {
            if (err) return done(err);
            assert('abc' == key);
            assert(123 == value);
            iterator.next(function (err, key, value) {
              assert(null === err);
              assert(undefined === key);
              assert(undefined === value);
              iterator.end(done);
            });
          });
        });
      });
    });

    describe('given a start key', function () {
      it('should throw when given a non-string', function () {
        var err = null;
        try {
          db.iterator({ reverse: true, start: {} });
        } catch (e) {
          err = e;
        }
        assert(err);
      });

      it('should start the iterator at the given key', function (done) {
        var iterator = db.iterator({ reverse: true, start: 'def' });
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert('abc' == key);
          assert(123 == value);
          iterator.next(function (err, key, value) {
            if (err) return done(err);
            assert(undefined === key);
            assert(undefined === value);
            iterator.end(done);
          });
        });
      });
    });
  });

  describe('given a start key', function () {
    var db;

    beforeEach(function (done) {
      db = new Sophist('./testdb');
      setup(db, done);
    });

    afterEach(function (done) {
      db.close(done);
    });

    it('should throw when given a non-string', function () {
      var err = null;
      try {
        db.iterator({ start: {} });
      } catch (e) {
        err = e;
      }
      assert(err);
    });

    it('should not error given a missing key', function (done) {
      var iterator = db.iterator({ start: 'nope' });
      iterator.next(function (err, key, value) {
        if (err) return done(err);
        assert(undefined === key);
        assert(undefined === value);
        iterator.end(done);
      });
    });

    it('should start the iterator at the given key', function (done) {
      var iterator = db.iterator({ start: 'def' });
      iterator.next(function (err, key, value) {
        if (err) return done(err);
        assert('ghi' == key);
        assert(789 == value);
        iterator.end(done);
      });
    });

    describe('given gte == true', function () {
      it('should start the iterator one step before the given key', function (done) {
        var iterator = db.iterator({ start: 'def', gte: true });
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert('def' == key);
          assert(456 == value);
          iterator.next(function (err, key, value) {
            if (err) return done(err);
            assert('ghi' == key);
            assert(789 == value);
            iterator.end(done);
          });
        });
      });
    });
  });

  describe('given an end key', function () {
    var db;

    beforeEach(function (done) {
      db = new Sophist('./testdb');
      setup(db, done);
    });

    afterEach(function (done) {
      db.close(done);
    });

    it('should throw when given a non-string', function () {
      var err = null;
      try {
        db.iterator({ end: {} });
      } catch (e) {
        err = e;
      }
      assert(err);
    });

    it('should not iterate past the key', function (done) {
      var iterator = db.iterator({ end: 'def' });
      iterator.next(function (err, key, value) {
        if (err) return done(err);
        assert('abc' == key);
        assert(123 == value);
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert(undefined === key);
          assert(undefined === value);
          iterator.end(done);
        });
      });
    });
  });

  describe('given a start and an end key', function () {
    var db;

    beforeEach(function (done) {
      db = new Sophist('./testdb');
      setup(db, function () {
        db.set('blah', 'blah', function (err) {
          if (err) throw err;
          db.set('blahblah', 'blahblah', done);
        });
      });
    });

    afterEach(function (done) {
      db.close(done);
    });

    it('should start and end appropriately', function (done) {
      var iterator = db.iterator({ start: 'def', end: 'blah' });
      iterator.next(function (err, key, value) {
        if (err) return done(err);
        assert('ghi' == key);
        assert(789 == value);
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert(undefined === key);
          assert(undefined === value);
          done();
        });
      });
    });
  });
});
