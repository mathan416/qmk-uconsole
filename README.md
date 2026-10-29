# uConsole QMK — fixed-gain trackball v5

Firmware for the uConsole's built-in keyboard and trackball. This personal
fork of [j1n6/qmk-uconsole](https://github.com/j1n6/qmk-uconsole), based on
**v2.1.1**, makes pointer movement predictable: movement depends on detected
sensor edges, without an extra boost for rolling faster. It replaces
upstream's pointer acceleration and coast with the **v5
fixed gain of 12.5 counts per detected edge**; scrolling retains upstream's
smoothing and momentum.

The fork's working branch is **`v5-fixed12p5`**. The original history and QMK
submodule pin are preserved; `main` retains the upstream branch.

## Download and install

Get the [tested default firmware ZIP](https://github.com/mathan416/qmk-uconsole/releases/download/v2.1.1-fixed12p5/uconsole-v5-fixed12p5-default.zip)
and unzip it. It contains the firmware, checksums and `INSTALL.md`; the
[full installation notes](https://github.com/mathan416/qmk-uconsole/releases/download/v2.1.1-fixed12p5/INSTALL.md)
are also available separately. The
[release page](https://github.com/mathan416/qmk-uconsole/releases/tag/v2.1.1-fixed12p5)
also has the matching source bundle, checksums and license notices.

### Updating an existing QMK installation

Before starting, keep stable power, a backup of your current firmware and an
external keyboard or SSH connection ready. Install `dfu-util` on the computer
doing the update and disconnect other DFU-capable devices.

If device identity is unclear, follow the full guide's `dfu-util -l` check
first. If multiple devices match, disconnect the others or use a verified USB
path as described there before writing.

1. Open a terminal in the unzipped folder and check the files:

   ```sh
   sha256sum -c SHA256SUMS
   ```

   On macOS, use `shasum -a 256 -c SHA256SUMS`. Continue only if the checks pass.

2. Run the update command below. It **writes keyboard firmware**, selecting
   DFU device **`1eaf:0003`**, alternate setting **2**:

   ```sh
   sudo dfu-util -w -d ,1eaf:0003 -a 2 -D uconsole-v5-fixed12p5-default.bin -R
   ```

3. While it waits, press **Left Alt + Right Alt + Start simultaneously** to
   enter the QMK bootloader. Read the complete result, then check typing,
   pointer movement, clicks, Select scrolling and precision after restart.

The DFU-only selector above follows the official manual; it has not been
physically tested here. See the full
installation notes for that distinction and [recovery options](docs/RECOVERY.md).

### Coming from stock firmware

First-time installation uses a different route. Start with the
[upstream stock-to-QMK guide](docs/UPSTREAM-v2.1.1.md#option-1-first-time-flash-from-stock-firmware).
That migration has not been retested for this release; the update steps above
apply to a keyboard already running QMK.

## Everyday controls

Set the host operating system's keyboard layout to **US English (ANSI)**.
Other layouts can produce different symbols, particularly with special
characters and tap-hold keys.

| Control | Action |
| --- | --- |
| Trackball | Move the pointer. |
| Hold **Select** and roll the trackball | Scroll vertically or horizontally. |
| Hold **Select** and click the trackball's middle button | Toggle precision pointer movement. |
| D-pad | Arrow keys in keyboard mode; joystick axes in gamepad mode. |
| Hold gamepad **Y** or **B**, then use the D-pad | Move the pointer while in keyboard mode. |
| **Fn + G** | Toggle gamepad mode. |
| **Fn + Space** | Cycle the keyboard backlight. |
| **Fn + Esc** | Toggle the keyboard lock; press again to unlock. |
| **Fn + T + H** | Toggle tap-hold typing in the default build. Disabled by default; the setting is saved. The `no_tap_hold` build omits it. |

With tap-hold enabled, a short tap types the base character; holding for at
least 200 ms types its shifted form. See the
[original keyboard guide](docs/UPSTREAM-v2.1.1.md) for the full mappings.

## Pointer behavior

- Normal movement: fixed **12.5 counts/edge**, with no firmware pointer
  acceleration or coast. Desktop pointer settings can still affect travel.
- Precision: exactly **2 counts/edge**, toggled with **Select + trackball click**.
- Half-counts carry across reports, idle and reversals. For example, two positive
  edges emit 12 then 13 counts; two negative edges emit -13 then -12. This is
  deterministic rounding to whole USB counts, not speed-dependent gain.
- Each normal report drains at most **10 edges (125 counts) per axis**; excess
  raw movement stays queued. Precision is applied when the queue drains.
  Transitions between pointer and Select-scroll modes, and suspend/wake,
  discard queued pointer movement and fractional carry. Toggling precision
  preserves the normal half-count remainder.
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
format and reproduction limits. Setup uses `sh scripts/init-source.sh` to fetch
the pinned target dependencies without the unused NXP SDK. After setup:

```sh
make test          # host tests of the actual firmware C sources
make build         # default keymap, build only
make no_tap_hold   # optional keymap, build only
```

The movement and scrolling tests pass. You can run them locally with
`make test`; automatic GitHub checks are currently disabled. See the
[verification notes](docs/VERIFICATION.md) for the full test and build results.

## Recovery and original project

Read [RECOVERY.md](docs/RECOVERY.md) before any future firmware update. Build
and test commands do not flash, and this branch has no `make reflash` target.
The upstream v2.1.1 README is preserved in
[UPSTREAM-v2.1.1.md](docs/UPSTREAM-v2.1.1.md) for keyboard features, attribution
and recovery background; its pointer acceleration description does not apply
to this v5 profile. Use this fork's build instructions for this branch.

## Attribution and licensing

This fork builds on work by **j1n6, the upstream contributors and QMK**.
The v5 changes and new fork materials use GPL-3.0. See the
[licensing notes](docs/LICENSING.md) for details and component notices.
