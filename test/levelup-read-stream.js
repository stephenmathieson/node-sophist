
var levelup = require('levelup');
var assert = require('better-assert');
var Sophist = require('..');
var util = require('./util');

describe('levelup(sophist).createReadStream', function () {
  var db;

  beforeEach(function (done) {
    util.openClearClose(function (err) {
      if (err) throw err;
      db = levelup('./testdb', { db: Sophist });
      db.open(function (err) {
        if (err) throw err;
        db.batch()
        .put('readable1', 'value')
        .put('readable2', 'value')
        .put('readable3', 'value')
        .put('readable4', 'value')
        .put('readable5', 'value')
        .put('readable6', 'value')
        .put('readable7', 'value')
        .write(done);
      });
    });
  });

  afterEach(function (done) {
    db.close(done);
  });

  it('should create a Readable stream', function (done) {
    var i = 0;
    db.createReadStream()
    .on('data', function (chunk) {
      i++;
      assert('readable' + i == chunk.key);
      assert('value' == chunk.value);
    })
    .on('end', function () {
      assert(i == 7);
    })
    .on('close', function () {
      assert(i == 7);
      done();
    });
  });

  describe('given `end`', function () {
    it('should iterate until that key', function (done) {
      var i = 0;
      db.createReadStream({ end: 'readable4' })
      .on('data', function (chunk) {
        i++;
        assert('readable' + i == chunk.key);
        assert('value' == chunk.value);
      })
      .on('end', function () {
        assert(i == 3);
      })
      .on('close', function () {
        assert(i == 3);
        done();
      });
    });
  });

  describe('given `start`', function () {
    it('should iterate until that key', function (done) {
      var i = 4;
      db.createReadStream({ start: 'readable4' })
      .on('data', function (chunk) {
        i++;
        assert('readable' + i == chunk.key);
        assert('value' == chunk.value);
      })
      .on('end', function () {
        assert(i == 7);
      })
      .on('close', function () {
        assert(i == 7);
        done();
      });
    });
  });

  describe('given `start` and `end`', function () {
    it('should iterate from `start` to `end`', function (done) {
      var i = 2;
      db.createReadStream({ start: 'readable2', end: 'readable6' })
      .on('data', function (chunk) {
        i++;
        assert('readable' + i == chunk.key);
        assert('value' == chunk.value);
      })
      .on('end', function () {
        assert(i == 5);
      })
      .on('close', function () {
        assert(i == 5);
        done();
      });
    });
  });
});
