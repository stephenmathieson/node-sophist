
# sophist

  A (maintained) Sophia binding.

[![Build Status](https://travis-ci.org/stephenmathieson/node-sophist.png?branch=master)](https://travis-ci.org/stephenmathieson/node-sophist)

## API

### var db = new Sophist(path);

  Create a db instance at `path`.

#### db.open([options], [fn]) / db.openSync([options])

  Open the database.

  Examples:

```js
yield db.open({ createIfMissing: false });
db.open(function (err) { /* ... */ });
db.openSync();
```

  Options:

  * `createIfMissing`: boolean, default `true`
  * `readOnly`: boolean, default `false`
  * `pageSize`: number, default `2048`
  * `mergeWatermark`: number, default `100000`

#### db.close([fn]) / db.closeSync()

  Close the database.

  Examples:

```js
yield db.close();
db.close(function (err) { /* ... */ });
db.closeSync();
```

#### db.set(key, value, [fn]) / db.setSync(key, value)

  Set `key` to `value` in the database.

  Examples:

```js
yield db.set('foo', 'bar');
db.set('foo', 'bar', function (err) { /* ... */ });
db.setSync('foo', 'bar');
```

#### db.get(key, [fn]) / db.getSync(key)

  Get the value of `key`.

  Examples:

```js
var value = yield db.get('foo');
db.get('foo', function (err, value) { /* ... */ });
var value = db.getSync('foo');
```

#### db.delete(key, [fn]) / db.deleteSync(key)

  Delete `key` from the database.

  Examples:

```js
var value = yield db.delete('foo');
db.delete('foo', function (err) { /* ... */ });
var value = db.deleteSync('foo');
```

#### var iterator = db.iterator()

  Create an iterator.

  **NOTE**: Sophia does *not* support writes while an iterator is open.

##### iterator.next([fn])

  Get the next key/value pair from the iterator.

  Upon reaching the last key, `null`s will be provided.

  Examples:

```js
var arr = yield iterator.next(); // [key, value]
iterator.next(function (err, key, value) { /* ... */ });
```

##### iterator.end([fn])

  End the iterator.
  Examples:

```js
yield iterator.end();
iterator.end(function (err) { /* ... */ });
```

#### var transaction = db.transaction()

  TODO

##### transaction.set(key, value)

  TODO

##### transaction.delete(key)

  TODO

##### transaction.commit(fn)

  TODO

## License

  MIT
