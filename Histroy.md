
# Release History

## 1.0.0 (September 18th, 2014)

  * **breaking change:** Removed `levelup` support entirely
  * **breaking change:** Removed `db.purge()` and `db.clear()`
  * **breaking change:** Upgraded Sophia to [5a88904](https://github.com/pmwkaa/sophia/commit/5a88904e282d7f89422a005204ea7cc5f1785486) ([#4](https://github.com/stephenmathieson/node-sophist/issues/4))
  * Add node 0.10.* and 0.11 support ([#5](https://github.com/stephenmathieson/node-sophist/issues/5) and [#11](https://github.com/stephenmathieson/node-sophist/issues/11))
  * Add more synchronous methods ([#12](https://github.com/stephenmathieson/node-sophist/issues/12)):
    - `db.openSync()`
    - `db.closeSync()`
    - `iterator.nextSync()`
    - `iterator.endSync()`
  * Add `yield` support to asynchronous methods:
    - `yield db.open()`
    - `yield db.close()`
    - `yield db.get(key)`
    - `yield db.set(key, value)`
    - `yield iterator.next()`
    - `yield transaction.commit()`
    - `yield transaction.rollback()`
  * `db.close()` and `db.closeSync()` will now cleanup any iterators left open

## 0.2.2 (April 20th, 2014)

 * Prevent segfaults when attempting to read/write an unopened db
 * Refactor to use [sophia.cc](https://github.com/stephenmathieson/sophia.cc)

## 0.2.1 (Feb 8th, 2014)

  * Remove `package.json` from deps/list to work around [npm#4630](https://github.com/npm/npm/issues/4630).

## 0.2.0 (Feb 8th, 2014)

  * Refactor transactions for nearly a 90% performance boost
  * Add `#deleteSync`
  * Change iteration end `null` -> `undefined`
  * Add primitive [LevelUP](https://github.com/rvagg/node-levelup) compatibility

## 0.1.0 (Jan 27th, 2014)

  * Fix error handling (NAN expects weird stuff with `errmsg`)
  * Add options to `#open`
  * Add `#getSync` and `#setSync`
  * Add `#purge`
  * Add iteration support:
      - `#iterator`
      - `iterator#next`
      - `iterator#end`
  * Add transaction support:
      - `#transaction`
      - `transation#commit`
      - `transation#rollback`
      - `transation#set`
      - `transation#delete`

## 0.0.0 (Jan 23rd, 2014)

  * Initial release, supporting very basic (and buggy) operations
