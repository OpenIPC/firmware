# OpenIPC firmware review standards

Rules for reviewing changes to OpenIPC/firmware — a Buildroot tree that builds images
for roughly 90 boards across 13 SoC vendors. `general/overlay/` and `general/package/`
are shared, so one file there reaches every camera of a family. Those cameras sit in
places nobody can physically reach, there is no staged rollout, and a bad change is
found only after `sysupgrade` has already written it to flash.

Each rule below was written from a pull request that was actually closed. The
referenced PR is the one that motivated it.

---

## 1. Blast radius: generic configuration is not your bench

The tree has no per-contributor scope. A file under `general/overlay/` is copied into
every image; a `load_<vendor>` script serves an entire SoC family. Weight a finding by
how many cameras the changed file reaches, not by how many lines the diff touches.

### 1.1 Never make your device-under-test the default in `general/overlay/`

`general/overlay/etc/init.d/S70vendor` runs on every OpenIPC camera. It deliberately
carries no board knowledge: it asks `ipcinfo -v` which vendor this is and hands off.
Pinning a sensor there overrides the autodetected sensor on every camera of every
vendor — including cameras already deployed, which pick the change up at upgrade time
and lose video.

```sh
# ❌ every camera now claims to have the contributor's sensor — #2241
load_"$vendor" -i -s imx662

# ✅
load_"$vendor" -i
```

Flag any diff under `general/overlay/` that introduces a sensor name, I2C address,
GPIO number, resolution, MAC prefix, or IP literal. Ask which boards the value is
wrong for; if the answer is "all but mine", it does not belong there.

### 1.2 A shared load script keeps its family default

`general/package/<vendor>-osdrv-*/files/script/load_<vendor>` sets the fallback sensor
for a whole SoC family. Changing that default to the sensor on the contributor's desk
silently repoints every other board in the family.

```sh
# ❌ hi3516cv6xx family default retargeted at one bench board — #2241
SNS_TYPE0=imx662;

# ✅ leave the family default; pass -s imx662 from the board's own configuration
SNS_TYPE0=sc4336p;
```

The same PR did contain the correct way to add a sensor — a new entry in the sensor
list the package already builds:

```make
# ✅ additive, changes no existing board's behaviour
HISILICON_OPENSDK_SENSORS_hi3516cv6xx = \
	...
	smart_sc500ai/libsns_sc500ai \
	sony_imx662/libsns_imx662
```

Editing a load script is not itself a problem — adding a sensor to one is normal and
expected. `#2248` added GC2023-over-MIPI to `hi3516cv200` by extending an existing case
arm, and changed no board's default:

```sh
# ✅ additive — merged as #2248
-        ov2710_mipi | ov2735_i2c_1080p | ov2735)
+        ov2710_mipi | ov2735_i2c_1080p | ov2735 | gc2023_mipi)
```

Flag only a change to an existing `SNS_TYPE*`, default resolution, or default profile in
a shared load script. Extending a case arm, adding a new `.ini`, or adding a sensor to a
package's sensor list changes no existing board and needs no such scrutiny.

### 1.3 A toolchain flag lands on every package in the image

`BR2_TARGET_OPTIMIZATION` is appended to `TOOLCHAIN_WRAPPER_OPTS` in Buildroot's
`toolchain/toolchain-wrapper.mk`, so it is baked into the compiler wrapper and reaches
every compilation unit the board builds — kernel modules, vendor SDK glue, busybox,
majestic, all of it. That is a whole-image ABI and codegen change, not a defconfig tweak,
and it is invisible in a diff that shows one added line per board.

(Buildroot is not vendored here. `make` downloads the version in `BR_VER` at the top of
the `Makefile` into `output-<board>/buildroot-$(BR_VER)/`, so read these files there.)

```
# ❌ no symptom named, no size or stability measurement — #2260
BR2_TARGET_OPTIMIZATION="-mno-unaligned-access"
```

Note this string is separate from the `-O` level, which comes from `BR2_OPTIMIZE_*` in
`package/Makefile.in`; do not claim one clobbers the other. That false finding has
already been made on a PR here.

Flag any added or changed `BR2_TARGET_OPTIMIZATION`, `BR2_TARGET_LDFLAGS`, or
`BR2_GLOBAL_PATCH_DIR` without a named symptom and a before/after image-size and
boot check on the affected board. If the flag fixes one package, fix that package.

---

## 2. Provenance of sources and binaries

Every board that selects a package inherits wherever that package points. Provenance
is therefore a whole-fleet property, not a detail of one contributor's workflow.

### 2.1 A package `SITE` points at OpenIPC, not a personal fork

A contributor's fork can be force-pushed, made private, or deleted, and every board
that builds the package breaks at the next clean build. It also puts unreviewed code
into images that carry the project's name.

```make
# ❌ every HiSilicon board now builds from one contributor's account — #2241
HISILICON_OPENSDK_SITE = $(call github,diegok3,openhisilicon,$(HISILICON_OPENSDK_VERSION))

# ✅
HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
```

Flag any `*_SITE` that names a user account rather than an OpenIPC-org repository or a
documented upstream project. The fix is to land the change in the upstream repo first
and then bump the version here.

### 2.2 A version bump must not make the pin less specific

The tree is not uniform here: most packages pin a full 40-character SHA
(`AWS_PRODUCER_VERSION`), `hisilicon-opensdk` has always used an 8-character short SHA,
and a number of packages still track `HEAD`. Full SHAs are preferred for anything new,
but a bump that keeps a package's established format is not a defect — `#2267` bumped
`ff20187b` to `2d637e35` and is the current convention for that package.

What is a defect is a bump that loosens the pin — fewer characters than the value it
replaces, or a branch name in place of a commit:

```make
# ❌ 8 hex digits replaced by 7, in the same PR that repointed SITE — #2241
HISILICON_OPENSDK_VERSION = 6c46247

# ✅ same format as the value it replaces
HISILICON_OPENSDK_VERSION = 2d637e35
```

Flag a `*_VERSION` that is shorter than the value it replaces, or that changes a commit
pin to a branch. Prefer a full 40-character SHA for a newly added package.

### 2.3 `Config.in` help text names the URL the `.mk` actually fetches

`#2241` repointed `HISILICON_OPENSDK_SITE` while the help text still told the reader
the code came from `OpenIPC/openhisilicon`. A reviewer or a downstream user checking
provenance reads the help text and gets the wrong answer.

Flag any `*_SITE` change whose corresponding `Config.in` help text, README, or comment
still names the previous source.

### 2.4 Binaries lifted from factory firmware are not a supply chain

`#2234` checked in 43 `.ko` modules pulled out of a camera's stock image, with a
`PROVENANCE.md` stating plainly that no source exists and no vendor SDK was obtained.
There is then no way to rebuild them for the next kernel, no way to fix a bug in them,
and no way to know they work on any board other than the one they came off. As the
maintainer put it when closing it: there is no way to build a functional streamer out
of extracted blobs and support it in the long run.

Flag any added `.ko`, `.so`, `.bin`, or firmware image that cannot be traced to a
vendor SDK release or a buildable source tree. A `PROVENANCE.md` documents the problem;
it does not solve it.

---

## 3. Repo boundaries

Most rejected work is not wrong — it is in the wrong repository. Redirecting a
contributor is a normal and useful review outcome, so name the target repo and why.

### 3.1 Kernel code and kernel patches belong to OpenIPC/linux

`general/package/all-patches/linux/` exists for patches that have already been through
kernel review. A new device-tree or driver patch landed here is invisible to everyone
working on the kernel and is silently dropped at the next kernel bump.

`#2234` added `0902-arm-dts-xm72050200-describe-the-watchdog-as-the-sp805-it-is.patch`
this way.

Flag any new file under `general/package/all-patches/linux/`. Redirect to
**OpenIPC/linux**; a patch that lands there needs no patch here.

### 3.2 Support for one retail camera model belongs to OpenIPC/builder

`general/overlay/usr/sbin/sysupgrade` already encodes this split: variants
`lite|ultimate|neo` upgrade from `OpenIPC/firmware`, everything else from
`OpenIPC/builder`. Board-specific work in this tree is dead weight for the other 89
boards and is not covered by this repo's CI matrix.

`#2234` added `general/package/goke-osdrv-gk7205v500/files/script/ircut_gk7201v200`,
a single-board IR-cut driver.

Four signals that a PR is mis-targeted:

1. the author says the package must not be enabled on generic SoC defconfigs;
2. no defconfig selects it, so it is dead code in this repo's CI;
3. it collides with an existing first-class package at the same installed path;
4. the author maintains a downstream board repo.

Flag these and redirect to **OpenIPC/builder**, under
`devices/common/br-ext-chip-<vendor>/`, with a per-board defconfig that selects the
package.

### 3.3 Probing and bring-up tools belong to OpenIPC/ipctool

`#2241` added `tools/imx662/vi_raw_capture.c`, `recv_raw.py`, `i2c_test.c`, and
`i2c_recovery.c` — about 1200 lines of sensor bring-up tooling. Nothing in the build
system referenced them, so they shipped in no image and CI never even compiled them.

Flag new diagnostic, capture, or register-poking utilities. Redirect to
**OpenIPC/ipctool**, which exists for exactly this and already ships to cameras.

### 3.4 Bugs in the streamer belong to majestic

Majestic is closed source with its own maintainers. A firmware-side workaround for a
majestic bug leaves the bug in place for every other platform and becomes permanent.

Flag changes to `general/package/majestic/files/*` that alter how majestic runs in
order to compensate for how majestic behaves. Redirect the contributor to file the
underlying issue with the majestic maintainers.

---

## 4. No monkey-patching

Each pattern below binds the image to one exact build of one vendor blob. None of them
fails loudly; they fail at the next vendor drop, in the field, on someone else's board.

### 4.1 `LD_PRELOAD` is never the fix

Preload shims are not acceptable anywhere in the OpenIPC tree — not in
OpenIPC/firmware and not in OpenIPC/builder. A shim intercepts a symbol for every
caller, is invisible to anyone debugging the process, and freezes the underlying bug
in place because nobody upstream ever sees it.

```sh
# ❌ — #2234, in general/package/majestic/files/S95majestic
if [ -e /dev/vpss_ctx_fix ] && [ -f /usr/lib/libgk_shim.so ]; then
	export LD_PRELOAD=/usr/lib/libgk_shim.so
fi
```

Flag any `LD_PRELOAD` in a shipped script, package file, or overlay. If the bug is in
majestic, it goes to majestic's maintainers; if it is in a vendor SDK, it goes to the
vendor SDK.

### 4.2 No runtime patching of vendor blob memory

`#2234` added a 948-line kernel module (`general/package/gk7201-fixup/src/gk7201_fixup.c`)
that reached into loaded vendor modules and rewrote object tables in their `.bss`. Those
offsets are valid for one build of one blob. The next vendor release moves them, and the
result is not a clean failure — it is memory corruption on a camera in a ceiling.

Flag any code that writes to another module's data section, patches an address derived
from `kallsyms`, or hooks a vendor symbol at runtime.

### 4.3 No generated facade libraries checked in as binaries

`#2234` shipped `gen_xmedia_shim.sh`, which emitted six `libhi_*.so` forwarding stubs
that were then committed as blobs. Checking in the generator does not make the binary
reviewable — nobody rebuilds it, and the committed `.so` is what actually ships.

Flag any committed binary produced by a script in the same PR. If it is generated, the
build system generates it; if the build system cannot, the change needs the real SDK.

---

## 5. Evidence

This project cannot test in CI what it most needs to know. The board matrix proves an
image builds; only the contributor's hardware can prove it boots and streams. That
makes the evidence in the PR description part of the change under review.

### 5.1 State the symptom before the fix

`#2260` changed a toolchain flag on two boards with no description of what was broken.
Without a symptom there is nothing to reproduce, nothing to regression-test, and no way
to tell whether the change helped or merely moved the failure.

Flag a behaviour-changing PR whose description names no symptom, no affected board, and
no way to observe the problem. "Improves stability" is not a symptom.

### 5.2 A test plan that was never run is worse than none

`#2260` shipped this, unchecked, and was closed for silence:

```
- [ ] Rebuild userspace; toolchain wrapper passes -mno-unaligned-access
- [ ] SSH + WPA2 stable on GK7102 (validated with rebuilt SDK toolchain)
```

An unchecked box asserts that verification was planned and implies it happened. It is
strictly worse than an honest "not tested", because it costs a reviewer the time to
discover otherwise.

Flag any PR description containing unchecked checkboxes under a test or verification
heading. Ask for the output, not the checkmark.

### 5.3 "Not tested on hardware" is not reviewable

A PR that states it was never run on a camera cannot be merged, and no amount of code
reading substitutes. This applies equally to hedges — "should work on", "untested but",
"in theory this also fixes".

Flag and close. The contributor is welcome to reopen with output from a real board.

---

## 6. Dead code and scope

### 6.1 New sources must be reachable from a defconfig

A package needs a `Config.in` entry, a `<pkg>.mk` that builds it, and at least one
defconfig that selects it. Without all three, CI never compiles the code, so nothing
proves it builds — let alone runs. `#2241`'s `tools/imx662/` had none of them.

Flag added `.c`/`.py`/script files that no `Config.in` selects and no `.mk` installs.
Either wire them up, or they belong in another repo (§3).

### 6.2 The diff must do what the title says

`#2241` was titled "Add Sony IMX662 sensor to build system". It also repointed
`HISILICON_OPENSDK_SITE` to a personal fork — a change affecting every HiSilicon board,
which no reviewer reading the title would look for.

Flag files in the diff that the stated purpose does not explain, especially shared
`.mk`, defconfig, and overlay files. Ask for them to be split into their own PR.

---

## 7. Shipped shell scripts

### 7.1 Portability is judged against busybox ash as built here, not against dash

`.github/workflows/shell-tests.yml` parse-checks every shipped script against busybox
ash, but only for syntax — `sh -n` does not execute, so a construct that parses can still
misbehave. That leaves a real gap for review, and it is tempting to close it by demanding
"no bashisms". Do not.

The target is not a minimal POSIX shell. The shipped busybox is built with
`CONFIG_ASH_BASH_COMPAT=y` (`general/package/busybox/busybox.config`), so `function
name()` and similar are valid on the device. Four scripts that work on real hardware —
both `hi3516cv6xx` and `hi3519dv500` `load_hisilicon`, `msc313e auto_run.sh`, and
`infinity6e zoom.sh` — are rejected by dash and run fine on cameras. Judging against a
stricter shell than the target turns working code red.

So raise portability only for constructs busybox ash genuinely lacks — bash arrays
(`arr=(...)`), `declare`/`typeset`, `${var^^}`/`${var,,}` case conversion, `${!var}`
indirect expansion, and herestrings (`<<<`) — and say which one you mean and why it
fails. This is a judgement call, deliberately not a compliance gate.

Flag a genuinely unsupported construct in a script under `general/overlay/` or
`general/package/*/files/`. Do not flag style, and do not flag anything under `.github/`
or `contrib/`, which run under bash away from the device.

---

## 8. Things that must not reach `master`

These are hard gates rather than judgement calls; `pr_compliance_checklist.yaml`
enforces them. Summarised here because they are the most common review findings:

- `LD_PRELOAD` in any shipped script, package file, or overlay.
- Binaries extracted from a camera's factory firmware, or any `.ko`/`.so`/`.bin` with
  no vendor SDK or buildable source behind it.
- New kernel patches under `general/package/all-patches/linux/` — those go to
  OpenIPC/linux.
- A `*_SITE` pointing at a personal fork, or a `*_VERSION` that is an abbreviated SHA.
- A sensor, GPIO, I2C address, or other board-specific value written into
  `general/overlay/` or into a shared `load_<vendor>` default.
- Single-board scripts and packages in the shared tree — those go to OpenIPC/builder.
- New sources that no `Config.in` selects and no defconfig builds.
- `insmod` where the tree uses `modprobe`, or an OpenSDK module not named `open_*`.
- A test plan whose boxes are unchecked, or a description stating the change was not
  tested on hardware.
