
var assert = require('assert');
var rmrf = require('rimraf');
var fs = require('fs');
var exists = fs.existsSync;
var Sophist = require('..');

describe('Sophist', function () {
  describe('#open(fn)', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function () {
      db = new Sophist('./test-db')
    });

    afterEach(function (done) {
      db.close(done);
    });

    it('should open create a new database', function (done) {
      db.open(function (err) {
        if (err) return done(err);
        assert(exists('./test-db'));
        done();
      });
    });
  });

  describe('#openSync()', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function () {
      db = new Sophist('./test-db')
    });

    afterEach(function () {
      db.closeSync();
    });

    it('should open create a new database', function () {
      db.openSync();
      assert(exists('./test-db'));
    });
  });

  describe('#close(fn)', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function () {
      db = new Sophist('./test-db')
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
    var db;

    beforeEach(cleanup);
    beforeEach(function () {
      db = new Sophist('./test-db')
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

  describe('#set(key, value, fn)', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function (done) {
      db = new Sophist('./test-db');
      db.open(done);
    });

    afterEach(function (done) {
      db.close(done);
    });

    it('should set a key', function (done) {
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

  describe('#setSync(key, value)', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function () {
      db = new Sophist('./test-db');
      db.openSync();
    });

    afterEach(function () {
      db.closeSync();
    });

    it('should set a key', function () {
      db.setSync('foo', 'bar');
      assert('bar' == db.getSync('foo'));
    });
  });

  describe('#get(key, fn)', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function (done) {
      db = new Sophist('./test-db');
      db.open(done);
    });
    beforeEach(function (done) {
      db.set('key1', 'value1', function (err) {
        if (err) return done(err);
        db.set('key2', 'value2', done);
      });
    });

    afterEach(function (done) {
      db.close(done);
    });

    it('should return the value of key', function (done) {
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
      it('should return null', function (done) {
        db.get('key3', function (err, value) {
          if (err) return done(err);
          assert(null == value);
          done();
        });
      });
    });
  });

  describe('#getSync(key)', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function () {
      db = new Sophist('./test-db');
      db.openSync();
    });
    beforeEach(function () {
      db.setSync('key1', 'value1');
      db.setSync('key2', 'value2');
    })

    afterEach(function () {
      db.closeSync();
    });

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

  describe('#delete(key, fn)', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function (done) {
      db = new Sophist('./test-db');
      db.open(done);
    });
    beforeEach(function (done) {
      db.set('key1', 'value1', function (err) {
        if (err) return done(err);
        db.set('key2', 'value2', done);
      });
    });

    afterEach(function (done) {
      db.close(done);
    });

    it('should remove key', function (done) {
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

  describe('#deleteSync(key)', function () {
    var db;

    beforeEach(cleanup);
    beforeEach(function () {
      db = new Sophist('./test-db');
      db.openSync();
    });
    beforeEach(function () {
      db.setSync('key1', 'value1');
      db.setSync('key2', 'value2');
    })

    afterEach(function () {
      db.closeSync();
    });

    it('should remove key', function () {
      db.deleteSync('key1');
      assert(null == db.getSync('key1'));
      assert('value2' == db.getSync('key2'));
    });
  });

  describe('#iterator()', function () {
    var db;
    var COUNT = 10;

    beforeEach(cleanup);
    beforeEach(function (done) {
      db = new Sophist('./test-db');
      db.open(done);
    });
    beforeEach(function () {
      for (var i = 0; i < COUNT; i++)
        db.setSync('key' + i, 'value' + i);
    })

    afterEach(function (done) {
      db.close(done);
    });

    it('should create an iterator', function () {
      var it = db.iterator();
      assert(it);
    });

    describe('#next(fn)', function () {
      var iterator;

      beforeEach(function () {
        iterator = db.iterator();
      });

      afterEach(function (done) {
        iterator.end(done);
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
      });
    });

    describe('#end(fn)', function () {
      it('should end the iterator', function (done) {
        var iterator = db.iterator();
        iterator.end(done);
      });
    });
  });
});


function cleanup(done) {
  rmrf('./test-db', done);
}
