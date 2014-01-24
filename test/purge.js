
var fs = require('fs');
var assert = require('better-assert');
var Sophist = require('..');

describe('Sophist#purge', function () {
  var db;

  beforeEach(function (done) {
    db = new Sophist('./testdb');
    db.open(done);
  });

  afterEach(function (done) {
    db.close(done);
  });

  it('should remove all temporary and incomplete data', function (done) {
    fs.writeFileSync('./testdb/foo.incomplete', 'foo');
    fs.writeFileSync('./testdb/bar.incomplete', 'bar');
    fs.writeFileSync('./testdb/baz.incomplete', 'baz');

    db.purge(function (err) {
      if (err) return done(err);
      assert(!fs.existsSync('./testdb/foo.incomplete'));
      assert(!fs.existsSync('./testdb/bar.incomplete'));
      assert(!fs.existsSync('./testdb/baz.incomplete'));
      done();
    });
  });

  it('should not corrupt the database', function (done) {
    db.set('hello', 'world', function (err) {
      if (err) return done(err);
      db.purge(function (err) {
        if (err) return done(err);
        db.get('hello', function (err, value) {
          assert('world' == value);
          done();
        });
      });
    });
  });
});
