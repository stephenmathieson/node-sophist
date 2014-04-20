
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#setSync', function () {
  var db;

  before(function () {
    db = new Sophist('./testdb');
  })

  describe('on an unopened database', function () {
    it('should throw', function () {
      var err;
      try {
        db.setSync('foo', 'bar');
      } catch (e) {
        err = e;
      }
      assert(err);
    });
  });

  describe('on an open database', function () {
    before(function (done) {
      db.open(done);
    });

    beforeEach(function (done) {
      db.clear(done);
    });

    after(function (done) {
      db.close(done);
    });

    it('should set key=value', function () {
      db.setSync('foo', 'bar');
      var value = db.getSync('foo');
      assert('bar' == value);
    });

    it('should throw errors', function (done) {
      var iterator = db.iterator();
      var err = null;

      try {
        db.setSync('foo', 'bar');
      } catch (e) {
        err = e;
      }

      assert(err);
      iterator.end(done);
    });
  });
});
