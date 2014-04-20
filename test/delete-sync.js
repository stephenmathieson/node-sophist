
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#deleteSync', function () {
  var db;

  before(function () {
    db = new Sophist('./testdb');
  });

  describe('on an unopened database', function () {
    it('should throw', function () {
      var err;
      try {
        db.deleteSync('foo');
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

    after(function (done) {
      db.close(done);
    });

    it('should delete a key/value pair', function () {
      db.setSync('foo', 'bar');
      db.deleteSync('foo');
      assert(null === db.getSync('foo'));
    });

    it('should handle numbers', function () {
      db.setSync(12, 'bar');
      db.deleteSync(12);
      assert(null === db.getSync(12));
    });

    it('should handle buffers', function () {
      db.setSync(new Buffer('foo'), 'bar');
      db.deleteSync(new Buffer('foo'));
      assert(null === db.getSync(new Buffer('foo')));
    });
  });
});
