
# sophist

  A (maintained) Sophia binding.

[![Build Status](https://travis-ci.org/stephenmathieson/node-sophist.png?branch=master)](https://travis-ci.org/stephenmathieson/node-sophist)

## API

### new Sophist(path)

  Initialize a Sophia binding for `path`.

#### Sophist#open([options], cb)

  Open/create the Sophia database with the given `options`, invoking `cb(err)`.

  Options:

  * `createIfMissing` - boolean, default `true`
  * `readOnly` - boolean, default `false`
  * `gc` - boolean, default `true`
  * `pageSize` - number, default `2048`
  * `mergeWatermark` - number, default `100000`

#### Sophist#close(cb)

  Close the Sophia database, invoking `cb(err)`.

#### Sophist#set(key, value, cb)

  Set `key=value` in the database, invoking `cb(err)`.

#### Sophist#setSync(key, value)

  Synchronously set `key=value`.

#### Sophist#get(key, cb)

  Get the value of `key`, invoking `cb(err, value)`.

#### Sophist#getSync(key)

  Synchronously get `key`.

#### Sophist#delete(key, cb)

  Delete `key` from the database, invoking `cb(err)`.

#### Sophist#count(cb)

  Get the number of keys in the database, invoking `cb(err, num)`.

#### Sophist#clear(cb)

  Clear all keys from the database, invoking `cb(err)`.

#### Sophist#purge(cb)

  Purge (delete) all incomplete / temporary data from the database.  This operation is **NOT** recoverable.

#### Sophist#iterator([options])

  Create an `Iterator` for the database.

  You may **not** write data while an iterator is open.

  Options:

  * `reverse` - boolean, default `false`
  * `start` - string, default `null`
  * `end` - string, default `null`
  * `gte` - boolean, default `false`

#### Iterator#next(cb)

  Get the next `key/value` pair from the database, invoking `cb(err, key, value)`.

  When the iterator has cycled through all data, will call `cb(null, null, null)`.

#### Iterator#end(cb)

  End the iteration cycle, invoking `cb(err)`.

## Credits

  Sophist is heavily inspired by [node-sophia](https://github.com/mmalecki/node-sophia) and [libsphia](https://github.com/sphia/libsphia).

## License 

(The MIT License)

Copyright (c) 2014 Stephen Mathieson &lt;me@stephenmathieson.com&gt;

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.