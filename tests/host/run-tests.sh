#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
CC=${CC:-cc}
test_output=$(mktemp -d "${TMPDIR:-/tmp}/uconsole-tests.XXXXXX")
trap 'rm -rf "$test_output"' EXIT HUP INT TERM
keyboard=clockworkpi/uconsole

# Compile the live firmware sources, with only QMK/platform APIs mocked.
"$CC" -std=c11 -Wall -Wextra -Werror -O2 -fsanitize=undefined \
  -Itests/host/mocks -I"$keyboard" \
  '-DTARGET_TRACKBALL="../../clockworkpi/uconsole/trackball.c"' \
  tests/host/direct_assertions.c "$keyboard/rate_meter.c" \
  "$keyboard/glider.c" "$keyboard/timeout.c" -lm -o "$test_output/assertions"
"$test_output/assertions"

# The fixture is byte-for-byte upstream v2.1.1 trackball.c. All other
# movement code is shared, so this compares wheel reports before/after v5.
for variant in upstream candidate; do
  case "$variant" in
    upstream) target=fixtures/upstream_trackball.c ;;
    candidate) target=../../clockworkpi/uconsole/trackball.c ;;
  esac
  "$CC" -std=c11 -Wall -Wextra -Werror -O2 \
    -Itests/host/mocks -I"$keyboard" "-DTARGET_TRACKBALL=\"$target\"" \
    tests/host/scroll_trace.c "$keyboard/rate_meter.c" \
    "$keyboard/glider.c" "$keyboard/timeout.c" -lm -o "$test_output/scroll-trace"
  "$test_output/scroll-trace" > "$test_output/scroll-$variant.txt"
done
cmp "$test_output/scroll-upstream.txt" "$test_output/scroll-candidate.txt"
test "$(wc -l < "$test_output/scroll-upstream.txt" | tr -d ' ')" = 11250
printf 'PASS: 11,250 wheel report records identical to upstream v2.1.1\n'
