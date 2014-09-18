
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

  describe('#open([fn])', function () {
    var db;
    var path = TEST_DB + '_';

    beforeEach(function () {
      db = new Sophist(path);
    });

    afterEach(function* () {
      yield db.close();
    });

    afterEach(function (done) {
      rmrf(path, done);
    });

    it('should be yieldable', function* () {
      yield db.open();
      assert(exists(path));
    });

    it('should open create a new database', function (done) {
      db.open(function (err) {
        if (err) return done(err);
        assert(exists(path));
        done();
      });
    });

    describe('with createIfMissing: false', function () {
      it('should fail to create a new db', function* () {
        var err;
        try {
          yield db.open({ createIfMissing: false });
        } catch (e) {
          err = e;
        }
        assert(err);
      });
    });
  });

  describe('#openSync()', function () {
    var db;
    var path = TEST_DB + '_';

    beforeEach(function () {
      db = new Sophist(path);
    });

    afterEach(function* () {
      yield db.close();
    });

    afterEach(function (done) {
      rmrf(path, done);
    });

    it('should be yieldable', function* () {
      yield db.open();
      assert(exists(path));
    });

    it('should open create a new database', function () {
      db.openSync();
      assert(exists(TEST_DB));
    });

    describe('with createIfMissing: false', function () {
      it('should fail to create a new db', function () {
        var err;
        try {
          db.openSync({ createIfMissing: false });
        } catch (e) {
          err = e;
        }
        assert(err);
      });
    });
  });

  describe('#close([fn])', function () {
    beforeEach(function* () {
      yield db.close();
    });

    it('should be yieldable', function* () {
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
      var err;
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

      it('should handle concurrent access', function (done) {
        var times = 1000;
        for (var i = 0; i < times; i++)
          db.get('key1', then);

        function then(err, value) {
          if (err) return done(err);
          assert('value1' == value);
          if (!--times) done();
        }
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

    it('should handle multiple iterators', function (done) {
      var n = 1000;
      for (var i = 0; i < n; i++) {
        var iterator = db.iterator();
        iterator.next(factory(iterator));
      }

      function factory(iterator) {
        return function next(err, key) {
          if (err) return done(err);
          assert('key0' == key);
          iterator.end(function (err) {
            if (err) return done(err);
            if (!--n) done();
          });
        };
      }
    });

    describe('with reverse: true', function () {
      it('should iterate in reverse', function* () {
        var iterator = db.iterator({ reverse: true });
        var set = yield iterator.next();
        assert.equal('key' + (COUNT - 1), set[0]);
        assert.equal('value' + (COUNT - 1), set[1]);
        yield iterator.end();
      });
    });

    describe('with start: <key>', function () {
      it('should get the key/value after <key>', function* () {
        var iterator = db.iterator({ start: 'key2' });
        var set = yield iterator.next();
        assert.equal('key3', set[0]);
        assert.equal('value3', set[1]);
        yield iterator.end();
      });

      describe('with gte: true', function () {
        it('should get <key>', function* () {
          var iterator = db.iterator({ start: 'key2', gte: true });
          var set = yield iterator.next();
          assert.equal('key2', set[0]);
          assert.equal('value2', set[1]);
          yield iterator.end();
        });
      });

      describe('if <key> is not a string', function () {
        it('should throw', function () {
          var err;
          try {
            db.iterator({ start: ['a', 'b', 'c'] });
          } catch (e) {
            err = e;
          }
          assert(err && 'start key must be a string' == err.message);
        });
      });
    });

    describe('with end: <key>', function () {
      it('should stop the iterator at <key>', function* () {
        var iterator = db.iterator({ end: 'key2' });
        var set = yield iterator.next();
        assert.equal('key0', set[0]);
        set = yield iterator.next();
        assert.equal('key1', set[0]);
        assert(null == (yield iterator.next()));
        yield iterator.end();
      });

      describe('if <key> is not a string', function () {
        it('should throw', function () {
          var err;
          try {
            db.iterator({ end: ['a', 'b', 'c'] });
          } catch (e) {
            err = e;
          }
          assert(err && 'end key must be a string' == err.message);
        });
      });
    });

    describe('with reverse, start, end, and gte', function () {
      it('should work', function* () {
        var iterator = db.iterator({
          reverse: true,
          start: 'key3',
          end: 'key1',
          gte: true,
        });
        assert.equal('key2', (yield iterator.next())[0])
        assert.equal(null, (yield iterator.next()));
        yield iterator.end();
      });
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
            if (err) return done(err);
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

    describe('#nextSync()', function () {
      var iterator;

      beforeEach(function () {
        iterator = db.iterator();
      });
      afterEach(function (done) {
        iterator.end(done);
      });

      it('should return [key, value]', function () {
        var arr = iterator.nextSync();
        assert.equal('key0', arr[0]);
        assert.equal('value0', arr[1]);
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

    describe('#endSync()', function () {
      it('should end the iterator', function () {
        var iterator = db.iterator();
        iterator.endSync();
      });
    });
  });

  describe('#transaction()', function () {
    it('should create a transaction', function () {
      var transaction = db.transaction();
      assert('Transaction' == transaction.constructor.name);
    });

    describe('with another transaction open', function () {
      it('should throw', function* () {
        var err;
        var t1 = db.transaction();
        try {
          db.transaction();
        } catch (e) {
          err = e;
        }
        assert(err && ~err.message.indexOf('transaction is already open'));
        yield t1.rollback();
      });
    });

    describe('#set(key, value)', function () {
      it('should add a pending set', function* () {
        var transaction = db.transaction();
        transaction.set('foo', 'bar');
        assert(null == (yield db.get('foo')));
        yield transaction.commit();
        assert('bar' == (yield db.get('foo')));
      });
    });

    describe('#delete(key)', function () {
      it('should add a pending delete', function* () {
        yield db.set('foo', 'bar');
        var transaction = db.transaction();
        transaction.delete('foo');
        assert('bar' == (yield db.get('foo')));
        yield transaction.commit();
        assert(null == (yield db.get('foo')));
      });
    });

    describe('#commit([fn])', function () {
      var transaction;

      beforeEach(function () {
        transaction = db.transaction();
        for (var i = 0; i < 100; i++) transaction.set(String(i), String(i));
      });

      it('should commit the pending writes', function (done) {
        transaction.commit(done);
      });

      it('should be yieldable', function* () {
        yield transaction.commit();
        assert('40' == (yield db.get('40')));
      });
    });

    describe('#rollback([fn])', function () {
      var transaction;

      beforeEach(function () {
        transaction = db.transaction();
        for (var i = 0; i < 100; i++) transaction.set(String(i), String(i));
      });

      it('should rollback the pending writes', function (done) {
        transaction.rollback(done);
      });

      it('should be yieldable', function* () {
        yield transaction.rollback();
        assert(null == (yield db.get('40')));
      });
    });
  });
});


function cleanup(done) {
  rmrf(TEST_DB, done);
}
