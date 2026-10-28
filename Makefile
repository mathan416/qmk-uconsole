.PHONY: default build no_tap_hold test
default: build no_tap_hold

build:
	sh scripts/build.sh default

no_tap_hold:
	sh scripts/build.sh no_tap_hold

test:
	sh tests/host/run-tests.sh
