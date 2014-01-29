
# Release History

## Unreleased (HEAD)

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