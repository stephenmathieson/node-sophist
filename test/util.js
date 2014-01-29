
var Sophist = require('..');

exports.openClearClose = function (db, fn) {
  if ('function' == typeof db) fn = db, db = new Sophist('./testdb');
  db.open(function (err) {
    if (err) return fn(err);
    db.clear(function (err) {
      if (err) return fn(err);
      db.close(fn);
    });
  });
};
