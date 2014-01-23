
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
