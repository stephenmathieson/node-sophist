
var Sophist = require('..');
var assert = require('better-assert');

describe('Sophist#close', function() {

  it('should close an open database', function (done) {
    var db = new Sophist('./testdb');
    db.open(function (err) {
      assert(!err);
      db.close(done);
    });
  });

  it('should require a callback', function () {
    var err;
    var db = new Sophist('./testdb');
    try {
      db.close();
    } catch (e) {
      err = e;
    }
    assert('Callback required' == err.message);
  });

  describe('closing an unopen database', function () {
    it('should produce handleable errors', function (done) {
      var db = new Sophist('./testdb');
      db.close(function (err) {
        assert(err);
        done();
      });
    });
  });
});
