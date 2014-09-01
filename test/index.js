
var assert = require('assert');
var rmrf = require('rimraf');
var fs = require('fs');
var exists = fs.existsSync;
var Sophist = require('..');

var TEST_DB = './test-db';

describe('Sophist', function () {
  var db;

  beforeEach(cleanup);
  beforeEach(function* () {
    db = new Sophist(TEST_DB);
    yield db.open();
  });

  afterEach(function* () {
    yield db.close();
  });

  describe('#open(fn)', function () {
    beforeEach(function* () {
      yield db.close();
    });

    it('should be yieldable', function* () {
      yield db.open();
      assert(exists(TEST_DB));
    });

    it('should open create a new database', function (done) {
      db.open(function (err) {
        if (err) return done(err);
        assert(exists(TEST_DB));
        done();
      });
    });
  });

  describe('#openSync()', function () {
    beforeEach(function* () {
      yield db.close();
    });

    it('should open create a new database', function () {
      db.openSync();
      assert(exists(TEST_DB));
    });
  });

  describe('#close([fn])', function () {
    beforeEach(function* () {
      yield db.close();
    });

    it('should be yield', function* () {
      yield db.open();
      yield db.close();
      var err;
      try {
        yield db.set('foo', 'bar');
      } catch (e) {
        err = e;
      }
      assert(err && 'Database not open' == err.message);
    });

    it('should close an opened database', function (done) {
      db.open(function (err) {
        if (err) return done(err);
        db.close(function (err) {
          if (err) return done(err);
          db.set('foo', 'bar', function (err) {
            assert(err && 'Database not open' == err.message);
            done();
          });
        });
      });
    });
  });

  describe('#closeSync', function () {
    beforeEach(function* () {
      yield db.close();
    });

    it('should close an opened database', function () {
      db.openSync();
      db.closeSync();
      try {
        db.setSync('foo', 'bar');
      } catch (e) {
        err = e;
      }
      assert(err && 'Database not open' == err.message);
    });
  });

  describe('#set(key, value, [fn])', function () {
    it('should set a key', function* () {
      yield db.set('foo', 'bar');
      var value = yield db.get('foo');
      assert('bar' == value);
    });

    describe('given a callback', function () {
      it('should work', function (done) {
        db.set('foo', 'bar', function (err) {
          if (err) return done(err);
          db.get('foo', function (err, value) {
            if (err) return done(err);
            assert('bar' == value);
            done();
          });
        });
      });
    });
  });

  describe('#setSync(key, value)', function () {
    it('should set a key', function () {
      db.setSync('foo', 'bar');
      assert('bar' == db.getSync('foo'));
    });
  });

  describe('#get(key, [fn])', function () {
    beforeEach(function* () {
      yield db.set('key1', 'value1');
      yield db.set('key2', 'value2');
    });

    it('should return the value of the key', function* () {
      var value = yield db.get('key1');
      assert('value1' == value);
      value = yield db.get('key2');
      assert('value2' == value);
    });

    describe('if key is missing', function () {
      it('should return null', function* () {
        var val = yield db.get('key3');
        assert(null == val);
      });
    });

    describe('given a callback', function () {
      it('should work', function (done) {
        db.get('key1', function (err, value) {
          if (err) return done(err);
          assert('value1' == value);
          db.get('key2', function (err, value) {
            if (err) return done(err);
            assert('value2' == value);
            done();
          });
        });
      });

      describe('if key is missing', function () {
        it('should still work', function (done) {
          db.get('key3', function (err, value) {
            if (err) return done(err);
            assert(null == value);
            done();
          });
        });
      });
    });
  });

  describe('#getSync(key)', function () {
    beforeEach(function () {
      db.setSync('key1', 'value1');
      db.setSync('key2', 'value2');
    })

    it('should return the value of key', function () {
      assert('value1' == db.getSync('key1'));
      assert('value2' == db.getSync('key2'));
    });

    describe('if key is missing', function () {
      it('should return null', function () {
        assert(null == db.getSync('key3'));
      });
    });
  });

  describe('#delete(key, [fn])', function () {
    beforeEach(function* () {
      yield db.set('key1', 'value1');
      yield db.set('key2', 'value2');
    });

    it('should remove key', function* () {
      yield db.delete('key1');
      var val = yield db.get('key1');
      assert(null == val);
      val = yield db.get('key2');
      assert('value2' == val);
    });

    describe('given a callback', function () {
      it('should work', function (done) {
        db.delete('key1', function (err) {
          if (err) return done(err);
          db.get('key1', function (err, value) {
            if (err) return done(err);
            assert(null == value);
            db.get('key2', function (err, value) {
              if (err) return done(err);
              assert('value2' == value);
              done();
            });
          });
        });
      });
    });
  });

  describe('#deleteSync(key)', function () {
    beforeEach(function () {
      db.setSync('key1', 'value1');
      db.setSync('key2', 'value2');
    })

    it('should remove key', function () {
      db.deleteSync('key1');
      assert(null == db.getSync('key1'));
      assert('value2' == db.getSync('key2'));
    });
  });

  describe('#iterator()', function () {
    var COUNT = 10;

    beforeEach(function () {
      for (var i = 0; i < COUNT; i++)
        db.setSync('key' + i, 'value' + i);
    });

    it('should create an iterator', function (done) {
      var it = db.iterator();
      assert(it);
      it.end(done);
    });

    describe('#next([fn])', function () {
      var iterator;

      beforeEach(function () {
        iterator = db.iterator();
      });

      afterEach(function (done) {
        iterator.end(done);
      });

      it('should be yieldable', function* () {
        var arr = yield iterator.next();
        assert('key0' == arr[0]);
        assert('value0' == arr[1]);
      });

      it('should get the next key', function (done) {
        iterator.next(function (err, key, value) {
          if (err) return done(err);
          assert('key0' == key);
          assert('value0' == value);
          done();
        });
      });

      describe('the last key', function () {
        beforeEach(function (done) {
          var pending = COUNT;
          iterator.next(next);
          function next(err) {
            if (!--pending) return done();
            iterator.next(next);
          }
        });

        it('should provide nulls', function (done) {
          iterator.next(function (err, key, value) {
            if (err) return done(err);
            assert(null == key);
            assert(null == value);
            done();
          });
        });

        it('should be provide null when yielded', function* () {
          var arr = yield iterator.next();
          assert(null == arr);
        });
      });
    });

    describe('#end([fn])', function () {
      it('should be yieldable', function* () {
        var iterator = db.iterator();
        yield iterator.end();
      });

      it('should end the iterator', function (done) {
        var iterator = db.iterator();
        iterator.end(done);
      });
    });
  });
});


function cleanup(done) {
  rmrf(TEST_DB, done);
}
