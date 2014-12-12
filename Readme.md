
# sophist

  A (maintained) Sophia binding.

[![Build Status](https://travis-ci.org/stephenmathieson/node-sophist.png?branch=master)](https://travis-ci.org/stephenmathieson/node-sophist)

## API

### var db = new Sophist(path);

  Create a db instance at `path`.

#### db.open([options], [fn]) / db.openSync([options])

  Open the database, optionally with the given `options`.

  Options:

  * `createIfMissing`: boolean, default `true`
  * `readOnly`: boolean, default `false`
  * `pageSize`: number, default `2048`
  * `mergeWatermark`: number, default `100000`

```js
yield db.open({ createIfMissing: false });
db.open(function (err) { /* ... */ });
db.openSync();
```

#### db.close([fn]) / db.closeSync()

  Close the database.

```js
yield db.close();
db.close(function (err) { /* ... */ });
db.closeSync();
```

#### db.set(key, value, [fn]) / db.setSync(key, value)

  Set `key` to `value` in the database.

```js
yield db.set('foo', 'bar');
db.set('foo', 'bar', function (err) { /* ... */ });
db.setSync('foo', 'bar');
```

#### db.get(key, [fn]) / db.getSync(key)

  Get the value of `key`.

```js
var value = yield db.get('foo');
db.get('foo', function (err, value) { /* ... */ });
var value = db.getSync('foo');
```

#### db.delete(key, [fn]) / db.deleteSync(key)

  Delete `key` from the database.

```js
var value = yield db.delete('foo');
db.delete('foo', function (err) { /* ... */ });
var value = db.deleteSync('foo');
```

#### var iterator = db.iterator([options])

  Create an iterator.

  **NOTE**: Sophia does *not* support writes while an iterator is open.

  Options:

  * `reverse`: boolean, default `false`
  * `start`: string, default `null`
  * `end`: string, default `null`
  * `gte`: boolean, default `false`
  * `lte`: boolean, default `false`

##### iterator.next([fn])

  Get the next key/value pair from the iterator.

  Upon reaching the last key, `null`s will be provided.

```js
var arr = yield iterator.next(); // [key, value]
iterator.next(function (err, key, value) { /* ... */ });
```

##### iterator.end([fn])

  End the iterator.

```js
yield iterator.end();
iterator.end(function (err) { /* ... */ });
```

#### var transaction = db.transaction()

  Create a Sophia transaction.

  During a transaction, all writes (`set` and `delete`) are posponed until `transaction#commit()` is called.  Transaction writes may be reverted by calling `transaction#rollback()`.

  Unlike Sophia's raw C API, values set by `transaction#set()` and `transaction#get()` are **not** able to be retreived by `sophist#get()`.

  Sophia does not support nested or multiple transactions, so doing so will cause an error to be thrown.

```js
var transaction = db.transaction();
var transaction2 = db.transaction(); // throws
```

##### transaction.set(key, value)

  Push a `set` operation into the transaction.

```js
yield db.set('foo', 'bar');
var transaction = db.transaction();
transaction.set('foo', 'baz');
var val = yield db.get('foo'); // bar
yield transaction.commit();
var val = yield db.get('foo'); // baz
```

##### transaction.delete(key)

  Push a `delete` operation into the transaction.

```js
yield db.set('foo', 'bar');
var transaction = db.transaction();
transaction.delete('foo');
var val = yield db.get('foo'); // bar
yield transaction.commit();
var val = yield db.get('foo'); // null
```

##### transaction.commit([fn])

  Commit the operations stored by the transaction.

```js
var transaction = db.transaction();
// ...
yield transaction.commit();
transaction.commit(function (err) { /* ... */ });
```

##### transaction.rollback([fn])

  Rollback/revert the operations stored by the transaction.

```js
var transaction = db.transaction();
// ...
yield transaction.rollback();
transaction.rollback(function (err) { /* ... */ });
```

## License

  MIT
