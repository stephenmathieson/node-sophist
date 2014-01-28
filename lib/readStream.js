var Readable = require('readable-stream').Stream;
var inherits = require('util').inherits;
var extend = require('xtend');

var defaultOptions = {key: true, value: true};

var makeKeyValueData = function(key, value) {
  return {
  	key : key,
  	value : value
  }	
};

var makeKeyData = function(key) {
  return key;	
};

function ReadStream(options, db, iteratorFactory) {
  if(!(this instanceof ReadStream))
  	return new ReadStream(options, db, iteratorFactory);

  Readable.call(this, {objectMode: true, highWaterMark: options.highWaterMark});

  this._options = extend(defaultOptions, options);

  this._iterator = iteratorFactory(this._options);

  this._makeData = this._options.key && this._options.value 
                     ? makeKeyValueData : this._options.key
                       ? makekeyData : null
}

inherits(ReadStream, Readable);

ReadStream.prototype._read = function() {
  var self = this;

  if(self._destroyed)
  	return;

  self._iterator.next(function(err, key, value) {
  	if(err || (key === undefined && value === undefined)) {
  	  if(!err && !self._destroyed) {
  	  	self.push(null);
  	  }

  	  return self._cleanup(err);
  	}

    value = self._makeData(key, value) || value;

  	if(!self._destroyed)
  	  self.push(value);	
  });
}

ReadStream.prototype._cleanup = function(err) {
  if(this._destroyed)
    return;

  this._destroyed = true;
  
  var self = this;

  if(err) {
  	self.emit('error', err);
  } 

  self._iterator.end(function() {
  	self._iterator = null;
  	self.emit('close');
  }); 	
}

module.exports = ReadStream;