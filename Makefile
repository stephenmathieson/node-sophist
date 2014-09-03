
BIN := node_modules/.bin
JS = $(wildcard index.js lib/*.js)
SRC = $(wildcard src/*.*)
TESTS = $(wildcard test/*.js)
REPORTER ?= spec

build: node_modules $(SRC)
	@node-gyp rebuild

node_modules: package.json
	@npm install
	@touch $@

clean:
	@node-gyp clean
	@rm -rf testdb test-*

test: build
	$(BIN)/gnode $(BIN)/_mocha \
	  --reporter $(REPORTER) \
	  --require co-mocha \
	  --slow 5ms

valgrind: build
	@valgrind \
	  --leak-check=full \
	  --trace-children=yes \
	    node --harmony \
	      node_modules/.bin/_mocha \
	        --reporter spec \
	        --require co-mocha


.PHONY: test clean valgrind
