
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');
var util = require('./util');

describe('levelup(sophist).createWriteStream', function () {
  var db;

  beforeEach(function (done) {
    util.openClearClose(function (err) {
      if (err) throw err;
      db = levelup('./testdb', { db: Sophist });
      db.open(done);
    });
  });

  afterEach(function (done) {
    db.close(done);
  });

  it('should create a Writable stream', function (done) {
    var ws = db.createWriteStream();

    ws.on('close', function (err) {
      if (err) throw err;
      db.get('name', function (err, value) {
        if (err) throw err;
        assert('Yuri Irsenovich Kim' == value);
        db.get('dob', function (err, value) {
          if (err) throw err;
          assert('16 February 1941' == value);
          db.get('spouse', function (err, value) {
            if (err) throw err;
            assert('Kim Young-sook' == value);
            db.get('occupation', function (err, value) {
              if (err) throw err;
              assert('Clown' == value);
              done();
            });
          });
        });
      });
    });

    ws.write({ key: 'name', value: 'Yuri Irsenovich Kim' });
    ws.write({ key: 'dob', value: '16 February 1941' });
    ws.write({ key: 'spouse', value: 'Kim Young-sook' });
    ws.write({ key: 'occupation', value: 'Clown' });
    ws.end();
  });
});
