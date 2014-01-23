
# sophist

  A (maintained) Sophia binding.

## API

### new Sophist(path)

  Initilize a Sophia binding for `path`.

#### Sophist#open(cb)

  Open/create the Sophia database, invoking `cb(err)`.

#### Sophist#close(cb)

  Close the Sophia database, invoking `cb(err)`.

#### Sophist#set(key, value, cb)

  Set `key=value` in the database, invoking `cb(err)`.

#### Sophist#get(key, cb)

  Get the value of `key`, invoking `cb(err, value)`.

#### Sophist#delete(key, cb)

  Delete `key` from the database, invoking `cb(err)`.

#### Sophist#count(cb)

  Get the number of keys in the database, invoking `cb(err, num)`.

#### Sophia#clear(cb)

  Clear all keys from the database, invoking `cb(err)`.

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