#!/bin/sh
# Fetch only the pinned dependencies used by the uConsole STM32 target.
set -eu
cd "$(dirname "$0")/.."
git submodule update --init -- qmk_firmware
git -C qmk_firmware submodule update --init -- \
  lib/chibios lib/chibios-contrib lib/lufa lib/printf
