
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');

describe('levelup(sophist).get', function () {
  var db;

  beforeEach(function (done) {
    db = levelup('./testdb', { db: Sophist });
    db.open(function (err) {
      if (err) return done(err);
      db.put('foo', 'bar', done);
    });
  });

  afterEach(function (done) {
    db.close(done);
  });

  it('should error if the db isn\'t open', function (done) {
    db.close(function () {
      db.get('foo', function (err) {
        assert(err);
        done();
      });
    });
  });

  it('should error if given a falsey key', function () {
    var err = null;

    try {
      db.get(null, function () {});
    } catch (e) { err = e; }
    assert(err);

    try {
      db.get('', function () {});
    } catch (e) { err = e; }
    assert(err);

    try {
      db.get(undefined, function () {});
    } catch (e) { err = e; }
    assert(err);

    try {
      db.get(function () {});
    } catch (e) { err = e; }
    assert(err);
  });

  it('should get values', function (done) {
    db.get('foo', function (err, value) {
      assert('bar' == value);
      done();
    });
  });

  it('should get values with options', function (done) {
    db.get('foo', { abc: 'def' }, function (err, value) {
      assert('bar' == value);
      done();
    });
  });
});
