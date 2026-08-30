# CLAUDE.md

Orientation for anyone — human or AI coding assistant — about to change this repository.
`AGENTS.md` is a symlink to this file, so Claude Code, Codex, Cursor, Copilot and the rest read
the same text.

This file tells you where things are and what will get a pull request closed. It does not replace
the review contract: `best_practices.md` holds the judgement calls, each one traced to a pull
request that was actually closed, and `pr_compliance_checklist.yaml` holds the binary gates. Read
`best_practices.md` before changing anything under `general/`.

## What this is

OpenIPC firmware is a Buildroot **external tree** that builds images for IP cameras across 13
SoC-vendor trees. Buildroot itself is **not vendored** — `make` downloads the version pinned in
`BR_VER` at the top of the `Makefile` into `$(TARGET)/buildroot-$(BR_VER)/`. When you need
Buildroot's own `package/`, `linux/` or `toolchain/` logic, read it there; it is not in this repo.

`make list` prints every board configuration. Do not trust a board count written down anywhere,
including here — the list moves.

One consequence drives every rule below. `general/overlay/` and `general/package/` are shared, so
a single file there reaches every camera of a family. Those cameras sit in places nobody can
physically reach, there is no staged rollout, and a bad change is discovered only after
`sysupgrade` has already written it to flash. Weight a change by how many cameras the file you
touched reaches, not by how many lines you changed.

## Before you start: is this the right repository?

The most common reason a well-made pull request gets closed here is that it belongs somewhere
else. Check this first — it costs a minute and can save an evening.

| What you are changing | Where it goes |
| --- | --- |
| Kernel source and kernel patches | [OpenIPC/linux][linux] |
| Support for one specific retail camera model | [OpenIPC/builder][builder] |
| Probing, identification, bring-up tooling | [OpenIPC/ipctool][ipctool] |
| A bug in the video stream itself | majestic's maintainers, not a shim here |
| Documentation, device notes, how-tos | [OpenIPC/wiki][wiki], [docs][docs] |

Kernel patches in particular: they go to OpenIPC/linux, never into
`general/package/all-patches/linux/`. The board *kernel config* is the exception that stays
here — `br-ext-chip-<vendor>/board/<family>/<model>.<variant>.config` belongs to this
repository, and turning a symbol on for one board is an edit to that file, not a kernel change.

What does belong here: shared packages, SoC-family drivers and load scripts, the root filesystem
overlay, board defconfigs for a whole SoC or variant, and the build system.

## Build

```sh
make BOARD=hi3516cv200_lite      # full build and repack for one board
make                             # whiptail menu of every board, then the same
make list                        # available board configurations
make package                     # available packages, as br-<name> targets
make deps                        # apt build dependencies (uses sudo) plus kconfiglib

make BOARD=<board> br-linux      # br-<x> runs Buildroot's own <x> target
make BOARD=<board> br-majestic-rebuild
make BOARD=<board> size-report   # per-file breakdown of what is in the image

make clean                       # drop build/images/target, keep buildroot and the dl cache
make distclean                   # drop everything, including the downloaded buildroot
```

`BOARD` is matched with `grep -m1` against `br-ext-*/configs/*_defconfig`, so a prefix works but
silently picks the first match — spell the board out. `TARGET` defaults to `$(PWD)/output`; if
you build more than one board locally, give each its own `TARGET=$(PWD)/output-<board>` or they
will contaminate each other.

## Common tasks

### Add a WiFi driver to a board

Most images ship no driver for your dongle because a driver is often 1.5 MB and a board may have
5120 KB of rootfs in total.

1. Find the package: `general/package/` carries `rtl8188fu-openipc`, `rtl8189fs-openipc`,
   `rtl88x2eu-openipc`, `aic8800-openipc`, `atbm-wifi` and others. `make package` lists them all.
2. Enable its symbol — `BR2_PACKAGE_<CHIP>_OPENIPC=y` — in the board's defconfig at
   `br-ext-chip-<vendor>/configs/<model>_<variant>_defconfig`.
3. Rebuild and read the size line the build prints. If it overflows, see *Make an image fit flash*.

Wiki: [adding-wifi-driver](https://github.com/OpenIPC/wiki/blob/master/en/adding-wifi-driver.md).
If this is for one retail camera rather than a whole SoC or variant, it belongs in OpenIPC/builder.

### Add a sensor to an SoC family

Additive only. Add an entry to the sensor list the package already builds — for example
`HISILICON_OPENSDK_SENSORS_<family>` in `general/package/hisilicon-opensdk/hisilicon-opensdk.mk`
— or add a case arm to the family's `load_<vendor>` script.

Do **not** repoint the family default (`SNS_TYPE0` in
`general/package/<vendor>-osdrv-*/files/script/load_<vendor>`) at the sensor on your desk. That
default serves every board in the family; changing it takes video away from all of them. This is
`best_practices.md` §1.2, written from #2241.

Wiki: [firmware-sensors](https://github.com/OpenIPC/wiki/blob/master/en/firmware-sensors.md),
[a worked example](https://github.com/OpenIPC/wiki/blob/master/en/firmware-sensor-install-sc223a.md).

### Add a board or a variant

1. `br-ext-chip-<vendor>/configs/<model>_<variant>_defconfig` is the entry point. The mainline
   variants are `lite`, `ultimate` and `neo`; a few vendor-specific ones exist.
2. Its kernel config lives at `br-ext-chip-<vendor>/board/<family>/<model>.<variant>.config`.
   Kernel sources come from OpenIPC/linux as a tarball named `<vendor>-<family>`.
3. **Register it with CI.** A new defconfig must be added to `ALL_BOARDS` in
   `.github/scripts/ci-matrix.py`, or to `UNBUILT_BOARDS` / `UNBUILT_FAMILIES` *with a reason*.
   `ci-matrix.py --self-test` is a merge gate and will fail the pull request otherwise.

Anything that should apply to every board goes in `general/openipc.fragment`, which `make
defconfig` concatenates with the board defconfig — not into a hundred separate files.

### Make an image fit flash

The caps are enforced by `CHECK_SIZE` in the `Makefile` and come from the image layout:

| Layout | Caps |
| --- | --- |
| 8 MB NOR | 2048 KB kernel + 5120 KB squashfs |
| 16 MB NOR | 2048 KB kernel + 8192 KB squashfs |
| NAND (UBI) | 16384 KB rootfs, plus a 4096 KB kernel on vendors that flash one separately |

`make BOARD=<board> size-report` shows what is taking the space. A board within 32 KB of its cap
prints a headroom warning — that is a real signal, not noise: several boards sit close enough
that one shared-overlay change tips them over.

Note that `general/scripts/rootfs_script.sh` will apply
`general/scripts/excludes/<model>_<variant>.list` if it exists, but this repository ships no such
directory. Those per-device prune lists live in OpenIPC/builder, alongside the device they trim.

### Change or add a package

`general/package/<name>/` holds `Config.in`, `<name>.mk`, and usually `files/`. Forks of upstream
Buildroot packages carry an `-openipc` suffix (`dropbear-openipc`, `libcurl-openipc`) so they do
not collide with Buildroot's own, and the symbol and variable prefixes follow
(`BR2_PACKAGE_DROPBEAR_OPENIPC`, `DROPBEAR_OPENIPC_SITE`).

A new package needs three things or it is dead code: a `Config.in`, a line sourcing it in
`general/package/Config.in`, and at least one defconfig that selects it. If nothing selects it on
purpose, add it to `NOT_BUILT` in `ci-matrix.py` with a reason, or `--self-test` fails.

## What the tree will not accept

Summarised from `pr_compliance_checklist.yaml`; the reasoning is in `best_practices.md`.

- **No `LD_PRELOAD`** in any shipped script, package file, or overlay. It intercepts a symbol for
  every caller, is invisible to anyone debugging the process, and freezes the real bug in place.
- **No binaries without buildable source.** Nothing lifted out of a factory image, nothing a
  script in the same pull request generated. A `PROVENANCE.md` documents the problem; it does not
  solve it.
- **No runtime patching of vendor blob memory**, and no `kallsyms` address hooking.
- **`*_SITE` points at an OpenIPC-org repository or a documented upstream**, never a personal
  fork, and a `*_VERSION` bump never becomes less specific than the pin it replaces — a full
  40-character SHA for anything new. `Config.in` help text must name the URL the `.mk` fetches.
- **No board-specific value in shared files.** A sensor name, I2C address, GPIO number,
  resolution, MAC prefix or IP literal does not belong in `general/overlay/` or in a shared
  `load_<vendor>` default. Extending a case arm, or adding a sensor to a package's list, is
  additive and fine.

## Verifying a change

### Without a camera

Build the board your change affects, then run the checks CI runs that need neither a build
nor a camera. There is no test framework; these are scripts, run them from the repository root:

```sh
bash .github/scripts/test_load_hisilicon.sh                   # os_mem_size derivation
bash .github/scripts/test_sysupgrade.sh                       # sysupgrade rootfs verification
bash .github/scripts/test_excludes_report.sh                  # excludes lists report stale entries
STRICT=1 bash .github/scripts/test_shell_parse.sh             # every shipped script parses
STRICT=1 bash .github/scripts/test_strip_shell_comments.sh    # ...and still parses once stripped
python3 .github/scripts/ci-matrix.py --self-test              # the selector agrees with the tree
python3 .github/scripts/lint-workflow-shell.py --self-test    # every workflow run: block parses
python3 general/scripts/tests/test_kconfig_graph.py           # needs kconfiglib (make deps)
```

Two more run only inside CI: `check_target_modules.sh` needs a finished rootfs and says so
in its own header, and `build-summary.py --self-test` only matters if you edit `build.yml`.

`ci-matrix.py --stdin` tells you which boards your change will build:

```sh
git diff --name-only origin/master | python3 .github/scripts/ci-matrix.py --stdin
```

### With a camera

CI proves an image builds. Only your board proves it boots and streams. The pull request template
asks for the symptom, the SoC and board, and before/after evidence — paste the actual output
(`dmesg`, `ipcinfo`, the stream behaviour, the image size). A description of the output is not
the output.

This applies to any change that can alter what the firmware does on a camera, which is
nearly all of them. Note it is behaviour, not bytes: every build stamps the commit SHA and
a timestamp into `/usr/lib/os-release`, so byte equality would make even a typo fix count. The exceptions are documentation, review configuration, repository metadata,
`CODEOWNERS`, and CI machinery that only selects, lints or tests. The test is what the file
can change, not where it lives; if that is unclear for your diff, assume it needs a board.

Three things look like exceptions and are not. A zero-board `ci-matrix.py --stdin` result
does not mean your diff reaches no image: the selector reports zero for every defconfig
outside its matrix, and some of those are real boards left out for build cost, so editing
one changes a shipped image with no CI coverage at all. A workflow that feeds a build input
changes image bytes — `build.yml` sets `BUILD_ID` and `BUILD_SHA`, which land in
`/usr/lib/os-release` in every rootfs. And the post-build machinery under `general/scripts/`
rewrites every rootfs the tree builds.

### First boot: the camera ships unclaimed

`general/overlay/etc/shadow` gives root an **empty hash field**, not a password. A password
committed to a public repository is not a credential, and the first-boot journey it produced
asked the user to type a secret everyone already knows before letting them choose a real one.

Until root has a password the camera is *unclaimed*: majestic streams nothing (RTSP 401, ONVIF
unauthorized, HTTP 401 everywhere but the setup page) and there is no shell. Setting the
password claims it. `/etc/shadow` is the only record of the state, read live, so whichever door
sets the password the other sees it at once. `firstboot` — `sysupgrade -n`, which wipes the
overlay — returns the camera to unclaimed for free.

The two doors:

- **Browser** — majestic serves `/setup.html` and `POST /setup` without authentication while
  unclaimed, and 404/403s them the moment there is a password.
- **SSH or serial** — `general/overlay/usr/sbin/openipc-claim` is root's login shell
  (`general/overlay/etc/passwd`). It refuses anything non-interactive, runs `passwd`, then puts
  `/bin/sh` back and steps aside. It is self-disabling — a camera claimed through the browser
  finds it transparent and repairs `/etc/passwd` on the next login — so a broken gate cannot
  lock anyone out of a camera that has an owner.

**A login shell must be listed in `/etc/shells`.** dropbear checks it through `getusershell()`
*before* running it, so an unlisted shell is refused at authentication and the gate never runs —
and, worse, can never disable itself, because the self-heal happens at login. That file is built
up by `TARGET_FINALIZE_HOOKS` (busybox appends `/bin/ash`, skeleton-init appends `/bin/sh`) and
the rootfs overlay is rsynced over the target *after* those hooks, so shipping an
`overlay/etc/shells` would silently freeze their list. `general/scripts/rootfs_script.sh`
appends to it instead, which runs after both.

This needs a majestic build carrying `/setup`. An image with the blank shadow and an older
majestic gives a camera with wide-open RTSP and an unreachable WebUI, so the three repositories
land in order: majestic, then majestic-webui's `setup.html`, then this.

### Flashing safety

A bad rootfs is discovered after it has been written. Do not flash an untested image onto a
camera you have no serial recovery path to, and read the wiki's
[flashing](https://github.com/OpenIPC/wiki/blob/master/en/equipment-flashing.md) and
[UART](https://github.com/OpenIPC/wiki/blob/master/en/serial_pins_uart.md) pages before you do it
the first time.

## Notes for AI assistants

- **Do not edit anything under `output/`** (or whatever `TARGET` points at). That is the
  downloaded Buildroot tree and the scratch build directory. It is not tracked, `make distclean`
  deletes it, and nothing you change there can ever appear in a pull request. Read it for
  reference; make your change in the external tree.
- **Shipped shell scripts are judged against the busybox `ash` built here**, which sets
  `CONFIG_ASH_BASH_COMPAT=y` in `general/package/busybox/busybox.config` — not against `dash`.
  Only genuinely unsupported constructs are portability problems: bash arrays, `declare`,
  `${var^^}`, `${!var}`, `<<<`. Scripts under `.github/` and `contrib/` run under bash off-device
  and are exempt.
- **Shipped shell scripts are comment-stripped at build time** by
  `general/scripts/strip-shell-comments.awk`. The file in git is not the file that runs on the
  camera; `test_strip_shell_comments.sh` is what proves the stripped form still parses.
- **Never write hardware evidence you did not observe.** Do not fill in the pull request
  template's *Hardware tested on* or *Evidence* sections, and do not tick its Scope boxes, on
  behalf of someone whose camera you cannot see. An unticked box is fine; a wrongly ticked one
  gets the pull request closed. If the change has not run on a camera, say so plainly and open it
  as a draft.
- **No AI-assistant footers** in commit messages or pull request descriptions.
- Comments in this tree explain *why*, and usually cite the pull request or field failure that
  motivated the code. Match that when you add one; do not narrate what the next line does.

## Layout

- `general/` is the `BR2_EXTERNAL` tree — `external.desc` names it `GENERAL`, and
  `general/external.mk` exports the `OPENIPC_*` variables and includes every `package/*/*.mk`.
- `make defconfig` concatenates the board defconfig with `general/openipc.fragment` (shared
  toolchain, ccache, hardening and overlay settings) into `$(TARGET)/openipc_defconfig`.
- `general/overlay/` is `BR2_ROOTFS_OVERLAY`, copied verbatim into every image.
- `general/scripts/rootfs_script.sh` is the post-build hook. It stamps `/usr/lib/os-release`,
  prunes `libstdc++` unless a config symbol needs it, applies the excludes list, then applies
  late overlays and hooks keyed by config symbol from `general/scripts/late-overlays.list` and
  `late-post-build-hooks.list`. That pair of list files is the sanctioned way to ship a file only
  when a package is enabled.
- `general/package/all-patches/` is `BR2_GLOBAL_PATCH_DIR` for host and target packages. Kernel
  patches are the exception — they go to OpenIPC/linux.
- `<vendor>-osdrv-<family>` packages carry the vendor SDK blobs, the kernel modules, and the
  `files/script/load_<vendor>` script that brings up drivers for a whole SoC family.
- `repack` in the `Makefile` picks the image layout from the config, enforces the size caps, and
  emits `openipc.<model>-<layout>-<variant>.tgz`.

## CI

`.github/scripts/ci-matrix.py` derives the build matrix from the paths a pull request touches,
using the real Kconfig `select` and `_DEPENDENCIES` graph. Recognising a path can only *narrow*
the matrix; shared code, the overlay, the build system and anything it has never heard of widen
back to everything. A misclassification therefore costs runner time, never coverage.

Two things worth knowing before you open the pull request:

- A **draft** pull request deliberately builds nothing. Mark it ready for review when you want
  the matrix to run.
- Documentation-only changes build nothing either, by design.

If you add a workflow job that costs runner minutes, keep the repository guard the existing jobs
carry, so that a fork does not run the full matrix on someone else's Actions bill.

## Commit and pull request style

`area: lowercase imperative summary` — the area is a package, a board, a subsystem, or `ci` /
`build`. For example:

```
sysupgrade: ship /ram, and stop rmdir turning it into a whiteout
gpio-motors: deliver sub-tick delays on HZ=100 kernels
```

Maintainers append the pull request number at merge time; you do not need to.

[builder]: https://github.com/OpenIPC/builder
[docs]: https://docs.openipc.org
[ipctool]: https://github.com/OpenIPC/ipctool
[linux]: https://github.com/OpenIPC/linux
[wiki]: https://github.com/OpenIPC/wiki
