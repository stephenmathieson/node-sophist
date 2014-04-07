
var Sophist = require('..');
var assert = require('better-assert');
var rimraf = require('rimraf');

// https://github.com/stephenmathieson/node-sophist/issues/6
describe('Sophist#get segfaults (gh-6)', function() {
  var db = null;

  before(function (done) {
    rimraf('./testdb', function (err) {
      if (err) return done(err);
      db = new Sophist('./testdb');
      db.open(function (err) {
        if (err) return done(err);

        // "1000 entries of some < 1k json"
        var t = db.transaction();
        for (var i = 0; i < 1000; i++) {
          // a bit over 1k after stringification (1326bytes)
          var obj = {
            foo: str(256),
            bar: str(256),
            baz: str(256),
            qax: str(256),
            wer: str(256),
          };
          t.set('key__' + i, JSON.stringify(obj));
        }

        t.commit(done);
      });
    });
  });

  after(function (done) {
    db.close(done);
  });

  it('should not segfault on random gets', function (done) {
    var count = 100;
    (function again() {
      var key = 'key__' + n(0, 999);
      db.get(key, function (err, value) {
        if (err) return done(err);
        var obj = JSON.parse(value);
        assert(obj.foo && obj.bar && obj.baz && obj.qax && obj.wer);
        if (--count) {
          again();
        } else {
          done();
        }
      });
    }());
  });
});

/**
 * Create string of `l` length.
 */

function str(l) {
  var s = '';
  for (var i = 0; i < l; i++) {
    s += String.fromCharCode(n(0, 26) + 97);
  }
  return s;
}

/**
 * Get random number between `min` and `max`.
 */

function n(min, max) {
  return ~~(Math.random() * max) + min;
}

