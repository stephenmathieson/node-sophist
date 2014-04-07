
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');
var util = require('./util');

describe('levelup(sophist).del', function () {
  var db;

  beforeEach(function (done) {
    util.openClearClose(function (err) {
      db = levelup('./testdb', { db: Sophist });
      db.open(done);
    });
  });

  afterEach(function (done) {
    db.close(done);
  });

  it('should delete a key', function (done) {
    db.put('foo', 'bar', function (err) {
      if (err) throw err;
      db.del('foo', function (err) {
        if (err) throw err;
        db.get('foo', function (err, value) {
          if (err) throw err;
          assert(!value);
          done();
        });
      });
    });
  });
});
