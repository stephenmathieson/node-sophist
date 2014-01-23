
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#get', function () {
  var db;

  before(function (done) {
    db = new Sophist('./testdb');
    db.open(done);
  });

  beforeEach(function (done) {
    db.clear(done);
  });

  after(function (done) {
    db.close(done);
  });

  it('should get a value', function (done) {
    db.set('foo', 'bar', function (err) {
      if (err) return done(err);
      db.get('foo', function (err, value) {
        assert('bar' == value);
        done();
      });
    });
  });

  it('should provide null for non-existant keys', function (done) {
    db.get('blahblah', function (err, value) {
      assert(null == err);
      assert(null == value);
      done();
    });
  });
});
