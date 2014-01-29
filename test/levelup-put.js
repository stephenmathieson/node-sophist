
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');

describe('levelup(sophist).put', function () {
  var db;

  beforeEach(function (done) {
    db = levelup('./testdb', { db: Sophist });
    db.open(done);
  });

  afterEach(function (done) {
    db.close(done);
  });

  it('should error if the db isn\'t open', function (done) {
    db.close(function () {
      db.put('foo', 'bar', function (err) {
        assert(err);
        done();
      });
    });
  });

  it('should error if given a falsey key', function (done) {
    db.put(null, 'value', function (err) {
      assert(err);
      db.put(undefined, 'value', function (err) {
        assert(err);
        done();
      });
    });
  });

  it('should error if given a falsey value', function (done) {
    db.put('key', null, function (err) {
      assert(err);
      db.put('key', undefined, function (err) {
        assert(err);
        done();
      });
    });
  });

  it('should put values', function (done) {
    db.put('foo1', 'bar', function (err) {
      if (err) throw err;
      db.get('foo1', function (err, value) {
        assert('bar' == value);
        done();
      });
    });
  });

  it('should put values with options', function (done) {
    db.put('foo2', 'bar', { options: 'are fun' }, function (err) {
      if (err) throw err;
      db.get('foo2', function (err, value) {
        assert('bar' == value);
        done();
      });
    });
  });
});
