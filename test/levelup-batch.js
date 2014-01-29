
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');

describe('levelup(sophist).batch', function () {
  var db;

  beforeEach(function (done) {
    db = levelup('./testdb', { db: Sophist });
    db.open(done);
  });

  afterEach(function (done) {
    db.close(done);
  });

  describe('given an array of operations', function () {
    it('should execute the operations', function (done) {
      var ops = [
          { type: 'del', key: 'father' }
        , { type: 'put', key: 'name', value: 'Yuri Irsenovich Kim' }
        , { type: 'put', key: 'dob', value: '16 February 1941' }
        , { type: 'put', key: 'spouse', value: 'Kim Young-sook' }
        , { type: 'put', key: 'occupation', value: 'Clown' }
      ];

      db.batch(ops, function (err) {
        if (err) throw err;
        db.get('occupation', function (err, value) {
          if (err) throw err;
          assert('Clown' == String(value));
          done();
        });
      });
    });
  });

  describe('given no arguments', function () {
    it('should create a chainable batch', function (done) {
      db.batch()
      .del('father')
      .put('name', 'Yuri Irsenovich Kim')
      .put('dob', '16 February 1941')
      .put('spouse', 'Kim Young-sook')
      .put('occupation', 'Clown')
      .write(function (err) {
        if (err) throw err;
        db.get('occupation', function (err, value) {
          if (err) throw err;
          assert('Clown' == value);
          done();
        });
      });
    });
  });
});
