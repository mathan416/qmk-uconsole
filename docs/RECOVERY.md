# Firmware backups and recovery

This fork records the v5 source and build procedure. Keep the known-working
v4, original v2.1.1 application and tested v5 image backed up independently.
Application images do not back up the MCU bootloader or EEPROM. A fresh build
is not automatically identical to a saved, tested image.

Before any future update, keep stable power and an independent input path
(SSH or an external keyboard), check the file hash and confirm device identity
with `dfu-util -l`. This setup has not flashed or changed the uConsole.

## Existing QMK bootloader path

The following commands **write application firmware** and are reference only.
Do not run them as part of testing, building or repository setup. Use the exact
verified file you intend to install. For the existing QMK bootloader, the
previously used command is:

```sh
sudo dfu-util -w -d 1eaf:0003 -a 2 -D uconsole_direct_test_v5_fixed12p5.bin -R
```

While dfu-util waits, **Left Alt + Right Alt + Start** enters the QMK bootloader.
Read the complete result and verify input after restart. Do not suppress a
nonzero exit status. The old upstream `make reflash` wrapper suppressed errors
with `|| true`; this branch's Makefile provides build/test targets only.

To restore the saved v4 application using the same known-working path:

```sh
sudo dfu-util -w -d 1eaf:0003 -a 2 -D rollback_direct_test_v4_fixed10.bin -R
```

Do not substitute an address such as `0x08000000`, erase, or unprotect commands.
The bootloader is flash-resident and is not repaired by an application backup.
If the application cannot enter it, consult the publisher's physical micro-USB
and S1 procedure in the recovery section of [the original guide](UPSTREAM-v2.1.1.md).
That stock restore procedure restores stock firmware, not the v4/v5 application.
It has not been retested during this fork setup.

See the [dfu-util manual](https://dfu-util.sourceforge.net/dfu-util.1.html)
and [upstream v2.1.1 instructions](https://github.com/j1n6/qmk-uconsole/blob/v2.1.1/README.md)
for background. Confirm the applicable recovery route before writing firmware.
