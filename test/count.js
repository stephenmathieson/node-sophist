
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#count', function () {
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

  it('should return the number of keys in the db', function (done) {
    db.set('foo', 'bar', function (err) {
      if (err) throw err;
      db.count(function (err, count) {
        assert(1 == count);
        done();
      });
    });
  });
});
