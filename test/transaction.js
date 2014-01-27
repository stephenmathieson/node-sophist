
var Sophist = require('..');
var assert = require('better-assert');

describe('Sophist#transaction', function () {

  var db;

  beforeEach(function (done) {
    db = new Sophist('./testdb');
    db.open(function (err) {
      if (err) return done(err);
      db.clear(done);
    });
  });

  afterEach(function (done) {
    db.close(done);
  });

  it('should throw if the db isn\'t open', function () {
    var db = new Sophist('./testdb');
    var err = null;
    try {
      db.transaction();
    } catch (e) {
      err = e;
    }
    assert(err);
  });

  it('should create a new transaction', function (done) {
    var transaction = db.transaction();
    assert(transaction);
    transaction.commit(done);
  });

  it('should throw if another transaction is active', function (done) {
    var transaction = db.transaction();
    assert(transaction);
    var err = null;
    try {
      db.transaction();
    } catch (e) {
      err = e;
    }
    assert(err);
    transaction.commit(done);
  });

  describe('Transaction#commit', function () {
    it('should commit the current transaction', function (done) {
      var transaction = db.transaction();
      transaction.set('foo', 'bar');
      assert(null === db.getSync('foo'));
      transaction.commit(function (err) {
        if (err) return done(err);
        assert('bar' == db.getSync('foo'));
        done();
      });
    });
  });

  describe('Transaction#rollback', function () {
    it('should disgard the current transaction operations', function (done) {
      var transaction = db.transaction();
      transaction.set('foo', 'bar');
      assert(null === db.getSync('foo'));
      transaction.rollback(function (err) {
        if (err) return done(err);
        assert(null === db.getSync('foo'));
        done();
      });
    });

    it('should remove entries written by db.set', function (done) {
      var transaction = db.transaction();
      db.set('foo', 'bar', function (err) {
        if (err) return done(err);
        transaction.rollback(function (err) {
          if (err) return done(err);
          assert(null === db.getSync('foo'));
          done();
        });
      });
    });

    it('should remove entries written by db.delete', function (done) {
      db.setSync('foo', 'bar');
      var transaction = db.transaction();
      db.delete('foo', function (err) {
        if (err) return done(err);
        transaction.rollback(function (err) {
          if (err) return done(err);
          assert('bar' == db.getSync('foo'));
          done();
        });
      });
    });
  });

  describe('Transaction#set', function () {
    var transaction;

    beforeEach(function () {
      transaction = db.transaction();
    });

    it('should create a pending set operation', function (done) {
      transaction.set('foo', 'bar');
      assert(null === db.getSync('foo'));
      transaction.commit(function (err) {
        if (err) return done(err);
        assert('bar' == db.getSync('foo'));
        done();
      });
    });

    it('should chain', function (done) {
      transaction
      .set('key1', 'value1')
      .set('key2', 'value2')
      .commit(function (err) {
        if (err) return done(err);
        assert('value1' == db.getSync('key1'));
        assert('value2' == db.getSync('key2'));
        done();
      });
    });
  });

  describe('Transaction#delete', function () {
    var transaction;

    beforeEach(function (done) {
      db.set('key1', 'value1', function (err) {
        if (err) return done(err);
        transaction = db.transaction();
        done();
      });
    });

    it('should create a pending delete operation', function (done) {
      transaction.delete('key1');
      assert(db.getSync('key1'));
      transaction.commit(function (err) {
        if (err) return done(err);
        assert(null === db.getSync('key1'));
        done();
      });
    });

    it('should chain', function (done) {
      transaction
      .delete('key1')
      .set('key2', 'value2')
      .delete('key2')
      .set('key3', 'value3')
      .commit(function (err) {
        if (err) return done(err);
        assert(null === db.getSync('key1'));
        assert(null === db.getSync('key2'));
        assert('value3' == db.getSync('key3'));
        done();
      });
    });
  });
});
