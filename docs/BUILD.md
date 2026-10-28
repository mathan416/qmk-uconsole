# Building the v5 firmware

The `v5-fixed12p5` branch starts at upstream tag `v2.1.1`, commit
`b3604770f5a1caedc318e90c8a5b6b65023a01d2`. Its firmware change is exactly
the trackball source used for the tested v5 image. Build, tests and flashing
are separate operations; none of the commands in this document flash a device.

## Source pins

| Component | Commit |
| --- | --- |
| QMK | `a0b15d08bc56ccbea9990dde884136425e9886f7` |
| ChibiOS | `8bd61b804303f1614d574546c2dd735eeabb09f5` |
| ChibiOS-Contrib | `8d863d9ee4eecea68ad8d15f7e7c2b451aea79d9` |
| printf | `c2e3b4e10d281e7f0f694d3ecbd9f320977288cc` |
| LUFA | `549b97320d515bfca2f95c145a67bd13be968faa` |

The gitlinks pin these dependencies. Use `git submodule update --init --recursive`,
without `--remote`. Do not run `qmk setup` or replace the submodule with a fresh
clone of the latest QMK. Inspect local modifications before rebuilding an image
you intend to install.

## Original v5 toolchain

The supplied build provenance records Arm GNU Toolchain **14.3.Rel1**, GCC
**14.3.1 20250623**, QMK CLI **1.2.0**, and dfu-suffix **0.11**.
The Python package versions are in [python-build-requirements.txt](python-build-requirements.txt).
The original compiler was the Linux x86_64 archive:

- [Official Arm archive](https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz)
- SHA-256: `8f6903f8ceb084d9227b9ef991490413014d991874a1e34074443c2a72b14dbd`

For the closest reproduction, use this toolchain on Linux x86_64, verify the
archive hash before extraction, and put its `bin` directory first on `PATH`.
The kit does not record the complete original OS/Python environment, so the
pins alone are not a promise of byte-for-byte reproducibility.

## Fresh checkout

Install Git, Python with venv/pip, GNU Make (`gmake` on macOS), a host C compiler,
the Arm compiler above and its bundled binutils, and dfu-suffix 0.11. QMK uses
dfu-suffix to package the output file; it does not access a device. The host compiler is for tests.
Use a virtual environment rather than changing system Python packages.

```sh
git clone --branch v5-fixed12p5 https://github.com/mathan416/qmk-uconsole.git
cd qmk-uconsole
git submodule update --init --recursive
python3 -m venv .venv
. .venv/bin/activate
python -m pip install -r docs/python-build-requirements.txt
arm-none-eabi-gcc --version
qmk --version
make test
make build
```

`make build` invokes `qmk compile -kb clockworkpi/uconsole -km default`
inside the pinned QMK checkout and creates the keyboard symlink if absent.
It always enters QMK's dependency-aware build, so editing `trackball.c`
cannot leave the outer Makefile serving an old image. It fails if the QMK
revision or keyboard symlink is unexpected; it never clones a floating branch.

Output: `qmk_firmware/.build/clockworkpi_uconsole_default.bin` and `.elf`.
`make no_tap_hold` builds that optional keymap; `make` builds both variants.
The user-approved v5 image used the **default** keymap.

## Application image and DFU suffix

For the original v5 default image the raw application was **42,100 bytes**.
QMK automatically appends a 16-byte DFU suffix for this keyboard, producing
the **42,116-byte** image. Do not append a second suffix. With dfu-suffix 0.11
installed, checking and copying QMK's output are file operations only:

```sh
mkdir -p build-output
cp qmk_firmware/.build/clockworkpi_uconsole_default.bin build-output/v5-fixed12p5.bin
dfu-suffix -c build-output/v5-fixed12p5.bin
shasum -a 256 build-output/v5-fixed12p5.bin
```

Original suffixed image SHA-256:
`5ee9eb31356541731347de408697fefed838638241c4f4bd36c5592c44e0001d`.
Original ELF application load range: `0x08002000` through `0x0800c474`
(exclusive). An image built with another compiler can have a different layout
and hash. Do not label such an image as the original tested binary.

No firmware binaries or releases are added by this fork setup.

## Tests and CI status

`make test` needs a C11 compiler supporting UndefinedBehaviorSanitizer, and
compiles the current `clockworkpi/uconsole` C sources against mocked QMK APIs.
It checks fixed gain, fractional conservation, reversal/report grouping,
precision, overflow, idle behavior and mode/suspend resets. It also compares
11,250 wheel reports with a byte-for-byte v2.1.1 trackball fixture.

GitHub Actions is disabled on this fork. The authenticated GitHub credential
lacks the `workflow` scope, so a prepared host-test workflow could not be
published. The inherited workflows remain in the source history/tree but
must not be enabled as-is: they use floating action/install sources and the
Pages workflow deploys a website. This fork setup neither publishes firmware
releases nor deploys a website. After a separate workflow-permission setup,
replace those inherited workflows with read-only host-test CI before enabling
Actions. Firmware builds should retain the pins above and remain separate
from publishing and flashing.

The tests do not emulate physical interrupts, USB transport or hardware recovery.
User testing established the preferred feel on one uConsole; that is not broad
hardware compatibility coverage.
