# uConsole QMK — fixed-gain trackball v5

A personal fork of [j1n6/qmk-uconsole](https://github.com/j1n6/qmk-uconsole),
based on **v2.1.1**, with the **v5 fixed 12.5 counts per detected edge** pointer
profile. This version was tried on one uConsole and its feel was accepted by
its owner on 2026-10-28. It is not a claim of testing across all hardware.

The fork's working branch is **`v5-fixed12p5`**. The original history and QMK
submodule pin are preserved; `main` retains the upstream branch. No firmware
binaries or releases are published as part of this setup.

## Pointer behavior

- Normal movement: fixed **12.5 counts/edge**, with no firmware pointer
  acceleration or coast. Desktop pointer settings can still affect travel.
- Precision: exactly **2 counts/edge**, toggled with **Select + trackball click**.
- Half-counts carry across reports, idle and reversals. For example, two positive
  edges emit 12 then 13 counts; two negative edges emit -13 then -12. This is
  deterministic rounding to whole USB counts, not speed-dependent gain.
- Each normal report drains at most 10 edges (125 counts); excess raw movement
  stays queued. Precision is applied when the queue drains. Mode changes and
  suspend/wake discard queued pointer movement and fractional carry.
- Select scrolling keeps upstream's smoothing/momentum behavior. Keyboard
  mappings, bootloader configuration and EEPROM layout are unchanged.

The firmware source differs from v2.1.1 only in
[`clockworkpi/uconsole/trackball.c`](clockworkpi/uconsole/trackball.c).
Its SHA-256 is
`552d7df3e4257da842a3785b54e9a23e91757dde4f74d627881d9de9ba354010`.
The tested default image's SHA-256 is
`5ee9eb31356541731347de408697fefed838638241c4f4bd36c5592c44e0001d`
(42,116 bytes including its DFU suffix). Keep that saved image and the known
working rollback images independently of this source repository.

## Build and test

See [BUILD.md](docs/BUILD.md) for exact source/toolchain pins, setup, output
format and reproduction limits. After setup:

```sh
make test          # host tests of the actual firmware C sources
make build         # default keymap, build only
make no_tap_hold   # optional keymap, build only
```

The imported suite passed **3,328,375 checks** with UndefinedBehaviorSanitizer,
and **11,250 wheel reports** matched upstream v2.1.1. Run these tests locally;
GitHub Actions is disabled on this fork pending workflow-permission setup.
Mock APIs cannot validate real interrupt timing, USB transport or recovery.

## Recovery and original project

Read [RECOVERY.md](docs/RECOVERY.md) before any future firmware update. Build
and test commands do not flash, and this branch has no `make reflash` target.
The upstream v2.1.1 README is preserved in
[UPSTREAM-v2.1.1.md](docs/UPSTREAM-v2.1.1.md) for keyboard features, attribution
and recovery background; its pointer acceleration description does not apply
to this v5 profile. Use this fork's build instructions for this branch.

## Attribution and licensing

Credit to **j1n6 and the upstream contributors**, QMK and its dependencies.
Upstream's README declares **GPL-3.0** but the v2.1.1 tree lacks its linked root
LICENSE file. [LICENSING.md](docs/LICENSING.md) records that discrepancy and
preserves component-specific notices without claiming one license for every
component. The v5 changes and new fork materials follow GPL-3.0.
