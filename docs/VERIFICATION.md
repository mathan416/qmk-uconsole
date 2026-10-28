# Fork setup verification — 2026-10-28

- The supplied tested v5 image was readable and matched SHA-256
  `5ee9eb31356541731347de408697fefed838638241c4f4bd36c5592c44e0001d`.
- Applying the kit's patch to upstream v2.1.1 produced a `trackball.c` identical
  to the kit's candidate source: SHA-256
  `552d7df3e4257da842a3785b54e9a23e91757dde4f74d627881d9de9ba354010`.
- The imported host tests were adapted to compile this repository's live C
  sources rather than a second frozen candidate copy. `make test` passed
  **3,328,375 checks** with C11, warnings-as-errors and UndefinedBehaviorSanitizer.
  All **11,250 wheel report records** matched the v2.1.1 fixture.
- The upstream test fixture and archived upstream README were compared
  byte-for-byte with the files from tag v2.1.1.
- QMK and its target dependencies matched the gitlinks listed in BUILD.md.
- The default firmware compiled, linked and packaged on macOS arm64 using
  the available Arm GCC **9.2.1 20191025**, QMK CLI **1.2.0**, the pinned
  Python dependencies, and dfu-suffix **0.11** built from its official v0.11
  source (commit `5134687894520780a999c473b7e63565cadf4aac`).
  Its suffix check passed for `1eaf:0003`; the image was **44,816 bytes**,
  SHA-256 `9dbbe92c2be889256392644be44c9f3448fe2fe17c664aea5ab3984a5a2b0041`.
  This verifies a fresh target build, not reproduction of the original GCC
  14.3 image. Neither this fresh image nor an automated hardware test was used
  to replace the user's working firmware.
- The optional no-tap-hold keymap also compiled and passed its suffix check:
  **44,364 bytes**, SHA-256
  `bd8fb49ada6e3fa32153ebfcf9d8db48d5caabeff987b4a85aa836fdb0f53668`.
- GitHub rejected the initial branch push because adding a host-test workflow
  required the credential's missing `workflow` scope. That rejected push did
  not create the branch. Workflow edits were excluded from the published
  source, and Actions was disabled on the new fork to prevent the inherited
  workflows from running. Tests remain available via `make test`.
- New public files were reviewed for credentials, private paths, Library
  references and accidental binaries. Only source, tests, build scripts and
  documentation were selected for publication.

No device was flashed, no binary release was created, and the original
v5 image has not been rebuilt byte-for-byte in this environment.
