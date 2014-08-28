
BIN := node_modules/.bin
JS = $(wildcard index.js lib/*.js)
SRC = $(wildcard src/*.*)
TESTS = $(wildcard test/*.js)
REPORTER ?= spec

build: node_modules $(SRC)
	@node-gyp rebuild

node_modules:
	@npm install
	@touch $@

clean:
	@node-gyp clean
	@rm -rf testdb test-*

test: build
	@$(BIN)/mocha --reporter $(REPORTER)

benchmarks:
	@$(BIN)/matcha

.PHONY: test clean benchmarks
