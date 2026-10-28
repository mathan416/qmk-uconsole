#!/bin/sh
# Build application firmware only. This script never accesses a device.
set -eu
cd "$(dirname "$0")/.."
keymap=${1:-default}
case "$keymap" in
  default|no_tap_hold) ;;
  *) printf 'Usage: sh scripts/build.sh [default|no_tap_hold]\n' >&2; exit 2 ;;
esac
expected_qmk=a0b15d08bc56ccbea9990dde884136425e9886f7
if [ ! -f qmk_firmware/Makefile ] ||
   [ "$(git -C qmk_firmware rev-parse HEAD)" != "$expected_qmk" ]; then
  printf 'Initialize the pinned sources with: git submodule update --init --recursive\n' >&2
  exit 1
fi
link=qmk_firmware/keyboards/clockworkpi
if [ ! -e "$link" ] && [ ! -L "$link" ]; then
  ln -s ../../clockworkpi "$link"
elif [ ! -L "$link" ] || [ "$(readlink "$link")" != ../../clockworkpi ]; then
  printf 'Expected %s to be a symlink to ../../clockworkpi; inspect it before building.\n' "$link" >&2
  exit 1
fi
cd qmk_firmware
# Always enter QMK's dependency-aware build, including after trackball edits.
qmk compile -kb clockworkpi/uconsole -km "$keymap"
