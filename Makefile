
MOCHA = ./node_modules/.bin/mocha
JSCOVERAGE ?= jscoverage
MOCHA_REPORTER ?= spec
JS = index.js $(wildcard lib/*.js)
SRC = $(wildcard src/*.*)
TESTS = $(wildcard test/*.js)

build: node_modules $(SRC)
	@node-gyp rebuild

test: build node_modules
	@$(MOCHA) --reporter $(MOCHA_REPORTER)

coverage.html: node_modules lib-cov $(TESTS)
	@SOPHIST_COV=1 \
		MOCHA_REPORTER=html-cov \
		$(MAKE) test > coverate.html

lib-cov: $(JS)
	@rm -rf $@
	$(JSCOVERAGE) lib $@

node_modules:
	@npm install

clean:
	@rm -rf lib-cov testdb
	@node-gyp clean

.PHONY: test clean
