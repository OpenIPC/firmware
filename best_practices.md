# OpenIPC firmware review standards

Rules for reviewing changes to OpenIPC/firmware — a Buildroot tree that builds images
for roughly 90 boards across 13 SoC vendors. `general/overlay/` and `general/package/`
are shared, so one file there reaches every camera of a family. Those cameras sit in
places nobody can physically reach, there is no staged rollout, and a bad change is
found only after `sysupgrade` has already written it to flash.

Each rule below was written from a pull request this project actually received. The
referenced PR is the one that motivated it — usually one that was closed, occasionally
one still open where the pattern is clear enough to write down now.

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

### 1.4 The per-device seam already exists — name it, do not just say "move it"

"This is board-specific, take it to OpenIPC/builder" is the correct verdict and half an
answer. The tree already carries a mechanism for every board-specific thing a retail
camera needs, and a contributor who has not been shown it invents a new init script
with the pin numbers typed into it.

`general/overlay/etc/init.d/S30customizer` is the entry point. On first boot it runs
`/usr/share/openipc/customizer.sh` once, guarded by `/etc/custom.ok`; on every boot it
runs `/usr/share/openipc/muxes.sh`, which is where a board's pinmux and GPIO presets
go. `/usr/share/openipc/gpio.conf` is the declarative pin map beside them — `button`,
`ircut1`, `ircut2`, `led1`, `led2`, `light_ir`, `light_wl`, `light_sensor`, `speaker`,
`usb`, `-1` for a pin the board does not have — read by consumers such as
`general/package/quirc-openipc/files/qrscan.sh`. Per-image pruning has a seam too:
`general/scripts/rootfs_script.sh` applies `general/scripts/excludes/<model>_<variant>.list`,
and `late-overlays.list` ships a file only when a config symbol is set.

All of it is per-device *in OpenIPC/builder*, at the same paths under
`devices/<board>/`: 96 devices ship a `customizer.sh`, 18 a `gpio.conf`, 15 a
`muxes.sh`, and the excludes lists live there too — this repository deliberately ships
no `general/scripts/excludes/` directory at all. The exclusion key is
`<model>_<variant>`, which is the *generic* board's key as well, so a list added here
prunes the family board and not only the contributor's camera.

`#2446` added `S01leds` and `S99leds` driving GPIO 0, 4 and 9 on every camera the tree
builds, a `customizer.sh` in the shared overlay branding every image an Imou Cue 2, and
a `general/scripts/excludes/hi3516ev200_lite.list` that took `libsns_imx307.so` and
`default.ini` away from the generic `hi3516ev200_lite` board.

When redirecting, say which of these the work becomes. Note also that none of
`gpio.conf`, `muxes.sh` or `customizer.sh` is documented in OpenIPC/wiki —
`en/gpio-settings.md` is a human-readable pin table only — so the contributor has had
no way to find them.

### 1.5 `devmem` in a shipped script is now a question, not a given

Writing an SoC register from `rc.local` used to be the only way to tell a camera that
something is soldered to a pad. It no longer is. A `devmem` line is also the worst
available way to do it: it is undone by anything that later exports the pad, and it is
lost at the next reflash, because nothing in the camera has been told the pad exists.

The camera can be told now. `/api/v1/pinmux` reports what every pad can be, what it is
currently, and which pads are already driven, and takes a selection back — that is what
the pins page in OpenIPC/majestic-webui draws (`www/a/mj-pins.js`), and the choice is
remembered rather than replayed from a boot script. `nightMode` drives an IR
illuminator, PWM lamp included, from configuration. `#2446` added a `devmem` block to
the shared `rc.local` programming PWM1 for its IR LED.

So ask what owns the pad before accepting a register write. A `devmem` line under
`general/overlay/` needs to say why configuration cannot express it, and — because
`rc.local` is shared — why every other camera should execute it.

A related trap: "additive, so it changes no existing board" is the right exemption for a
`load_<vendor>` sensor case arm, and it is not a licence for the shared overlay.
`general/overlay/etc/wireless/usb` has accumulated 46 arms keyed on retail model names
in 327 lines, while 18 builder devices override the whole file. Treat those 46 as
grandfathered, not as precedent: a new arm is a board-specific value in
`general/overlay/` and belongs in the device's own copy of the file.

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

Do not read that list of extensions as the definition. `#2446` added
`general/overlay/etc/ir/nrxset`, an executable with no extension at all, and it went
unremarked through an automated review that reported it as "the supplied patch contains
no textual changes" — which is exactly what a binary looks like to anything reading the
diff as text. The signal that always survives is the diff marker itself:

```
Binary files /dev/null and b/general/overlay/etc/ir/nrxset differ
```

Any hunk rendered that way is a binary, whatever it is called and wherever it sits, and
`general/overlay/` is never the right place for a compiled artefact — the overlay is
copied verbatim into every image, so a blob there ships to every camera of every vendor.

### 2.5 A register table lifted from a vendor's driver is the same problem as a binary

§2.4 is about what a file *is*; this is about what it *contains*. A sensor init sequence
copied out of a vendor's shipped driver has no more provenance than the driver did — it
cannot be corrected against a datasheet nobody has, and the reasoning behind any one
register is gone. Written as C it passes every check aimed at blobs.

`#2446` replaced the SC2235 init table with a "Dahua DVP register sequence (114
entries)" whose only stated origin was Dahua's own firmware, dropping about forty
registers the OpenSDK driver sets and adding others, for every Hi3516EV200 board using
that sensor.

Flag an added or replaced register table that names a camera vendor rather than a
datasheet as its source. Ask for the deltas the board actually needs — here the PCLK
output-enable and pad-drive registers the PR itself identifies — rather than a wholesale
swap, and ask which other boards were retested.

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

### 3.5 SoC driver behaviour belongs to the SDK repository

`hisilicon-opensdk` fetches **OpenIPC/openhisilicon**, and the Sigmastar sensor drivers
come from **OpenIPC/sensors**. Both are OpenIPC-org repositories that take pull
requests. A sensor that mis-detects, an ISP that does not track gain, a driver that
leaves a pad unconfigured — those are changes to the driver, made once, for every tree
that consumes it.

`#2446` is the shape to recognise. Its `rc.local` waited twelve seconds, wrote five
SC2235 registers over I²C, then killed and relaunched majestic, and the comment above it
said what it was for: the sensor's DVP pad enables, and majestic not re-reading
orientation. Both halves name an owner. The pad enables are a two-line change to
`libraries/sensor/hi3516ev200/smart_sc2235/sc2235_sensor_ctl.c` in
**OpenIPC/openhisilicon**; the orientation re-read is an issue for **OpenIPC/majestic**.
Neither is a shell script that runs on every camera the tree builds.

A comment that explains *why* a workaround is needed has usually named the repository
the work belongs to. Read it as a redirect and quote it back.

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

### 4.4 A patch against an OpenIPC package is a bridge, not a substitute

Patches in a package directory are normal here. Twenty-two of the twenty-three
patched packages target code this project cannot commit to — ffmpeg, mbedTLS, vtund,
baresip and its libre, siproxd, ZeroTier, libwebsockets, f2fs-tools, mini-snmpd, uacme,
nabto, mavlink-router, onvif-simple-server, gst-plugins-bad, and the Realtek WiFi
drivers — because for those a downstream patch is the only route there is.

A patch against code the project *does* own is different. It is invisible to anyone
reading that repository, it is dropped the moment someone bumps `*_VERSION`, and every
other consumer of the code keeps the bug. So the default is a pull request to the owning
repository, and a `*_VERSION` bump here once it lands.

**The default has one legitimate exception, and the tree contains exactly one instance
of it.** `libevent-openipc` carries `0001-CMakeLists-remove-epoll_pwait2-check.patch`
against `https://github.com/OpenIPC/libevent`. That is the shape the exception should
take: one hunk, a build fix, obviously temporary, riding ahead of a bump. Do not read
this section as "never" — read it as "not instead of the pull request".

`#2446` is the other shape.
`general/package/hisilicon-opensdk/0001-sc2235-replace-init-table-with-dahua-dvp-sequence.patch`
rewrites a 114-entry sensor init table for every Hi3516EV200 board using that sensor.
`HISILICON_OPENSDK_SITE` is `$(call github,openipc,openhisilicon,...)`, the file it
patches is checked in there, and nothing about it is temporary.

So the question to ask of a new `*.patch` in a package whose `*_SITE` resolves to an
`openipc` repository is not whether it exists but whether it is a bridge: does the PR
name the pull request opened against the owning repository, is the patch the minimal
delta rather than a wholesale replacement (§2.5), and will it be deleted at the next
bump? Three yeses and it is the libevent case. Any no and it belongs upstream first.

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

### 5.3 "Not tested on hardware" is not reviewable — when the change reaches a camera

A PR that changes what an image contains and states it was never run on a camera cannot
be merged, and no amount of code reading substitutes. This applies equally to hedges —
"should work on", "untested but", "in theory this also fixes".

Flag and close. The contributor is welcome to reopen with output from a real board.

The rule is scoped to changes that can reach an image, because that is the thing only
hardware can settle. Documentation, review configuration, repository metadata, and the
board-selection logic in `ci-matrix.py` alter no image byte, so there is no before and
after to observe; `#2330` was flagged for saying so, having changed the selector and
nothing else. Asking for a dmesg paste there does not raise the standard, it invites a
paste from a board that was not exercising the change — the fabricated evidence §5.2
exists to catch.

The test is whether the diff can change what the firmware *does* on a camera — not where
the file lives, and not whether image bytes differ. Bytes are the wrong measure: every
build stamps `BUILD_SHA`, `BUILD_ID` and a timestamp into `/usr/lib/os-release`, so a
documentation commit changes bytes in every image too. Those provenance stamps do not
count. That phrasing is deliberate: three separate attempts to *enumerate* what reaches an
image each missed something, and each miss was a hole in the gate, while enumerating the
exempt side instead immediately started demanding camera output for `CODEOWNERS`. A
property holds where a list does not. Where the property is genuinely unclear, the change
is not exempt — the same fail-safe direction `ci-matrix.py` takes when an unrecognised
path widens the matrix instead of narrowing it.

Exempt in practice: documentation, review configuration, repository metadata, the pull
request template, `CODEOWNERS`, and the CI machinery that only selects, lints or tests.

The misses are worth naming, because they all look exempt and are not:

A zero-board result from `ci-matrix.py --stdin` does **not** mean a diff reaches no image.
The selector returns zero for every defconfig outside its matrix, and several of those are
real boards excluded only for build cost — `fh8852v210_lite` is a real lite firmware whose
entry reads "internal toolchain: builds gcc+musl from source". Editing such a defconfig
changes a kernel, a rootfs and a shipped image while the selector reports nothing. Those
boards need *more* evidence than the ones CI builds, not less, because CI supplies none.

A workflow is not automatically exempt. `build.yml` sets `BUILD_ID` and `BUILD_SHA`, which
`general/scripts/rootfs_script.sh` writes into `/usr/lib/os-release` in every rootfs it
builds. A workflow that feeds a build input can change image bytes.

Nor is the post-build machinery. `general/scripts/rootfs_script.sh` prunes `libstdc++`,
applies the excludes lists, and copies in the late overlays named by `late-overlays.list`
and `late-post-build-hooks.list`; `strip-shell-comments.awk` rewrites every shipped script.
None of those is a package, a defconfig or an overlay file, and all of them rewrite the
rootfs of every board that builds.

What an exempt change still owes is the evidence its own class admits — the self-test that
covers it, the selector output, the parse run — and the honesty half of the rule does not
move. Claiming hardware testing that did not happen fails whatever the diff touches.

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

### 6.3 Nothing ships that nothing runs

§6.1 catches source nobody compiles. Its mirror image is a file that reaches the rootfs
perfectly well and that nothing on the camera ever opens. `general/overlay/` needs no
`Config.in` and no `.mk` — anything dropped in it ships — so the usual dead-code check
never fires, and the cost lands on every board of every vendor, several of which sit
within 32 KB of their squashfs cap.

`#2446` added `general/overlay/etc/ir/nrxset` and
`general/overlay/etc/ir/nrx_night_06.txt`. Nothing in firmware or builder reads either
path, and the `rc.local` the same PR ships never invokes `nrxset` — while the comment
above that code says it does. Its own two comments also disagree with each other about
whether the files are needed at all: `rc.local` says the 3DNR parameters are applied
from them and not from the IQ profile, the IQ profile says the opposite and that the
parameters were baked into it instead. Both cannot be true, and either way one of the
two is dead.

**Reachability is semantic, not a literal path match, and getting that wrong turns this
rule into a false-finding generator.** Most of the tree's data files are never named by
a consumer. `hisilicon-osdrv-hi3516ev200.mk` installs `files/sensor/config/*.ini` into
`/etc/sensors/` by wildcard, and the file that gets used is chosen at runtime from the
configured sensor name; a kernel module is reached by `modprobe <name>`, not by path; an
IQ profile is named through `isp.iqProfile`. All of those are reachable. The compliance
checklist already says as much under "New sources are wired into the build" — a data
file landing under a path an existing install rule globs needs no `.mk` change and
passes — and this rule must not contradict it.

So trace the mechanism before raising anything. A file is reachable if something names
its path, or its basename, or constructs its name at runtime, or picks it up through an
install glob that a documented convention then selects from. The finding is for the
residue: a file whose name appears nowhere, that no convention selects, and that sits in
a directory no consumer knows about — `/etc/ir/` being the case in hand. Where a comment
asserts a consumer, check that the consumer is actually called; a stale comment is how a
dead file keeps looking justified. "The vendor's firmware had it" is not an answer.

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


### 7.2 A `cli` setting path is unvalidated, and signalling majestic has one right signal

Two mistakes around `cli` are silent on the camera and cheap to catch in review.

**The path.** `cli -s` writes into `/etc/majestic.yaml` through yaml-cli, which stores
whatever dotted path it is handed — creating the intermediate mappings as it goes — and
exits 0. majestic then ignores a key it does not recognise. So a mistyped path applies
nothing and reports nothing, for the life of the device.
`OpenIPC/builder`'s `t40_lite_movols-mo-805p` shipped six of them behind a trailing colon
(`cli -s .video0.bitrate: 4000`), and bitrate, rate-control mode, profile, GOP size, GOP
mode and OSD size never applied on that camera. Read the path, not just the value:
every component should look like a YAML key, with no trailing separator and no empty
component. OpenIPC/builder now lints exactly this in CI.

**The signal.** majestic reloads on `SIGHUP`, which is `killall -1 majestic` or
`/etc/init.d/S95majestic reload`. `infinity6e`'s `zoom.sh` sends `killall -10` in nine
places; signal 10 is `SIGUSR1`, which majestic's bundled thread pool catches to park a
thread and never resumes (OpenIPC/firmware#2365), so those crops were never applied.

And a signal is only safe once majestic can catch it. `S95majestic` starts it with
`start-stop-daemon -b`, which returns at the fork, so the process is visible to `pidof`
well before `main()` installs a `SIGHUP` handler — and until then the default action for
`SIGHUP` is to terminate. Measured on a hi3516ev200, the process appears with `SigCgt`
still `0000000000000000`. A boot-time script that writes config and signals immediately
can therefore take the streamer away for the rest of the boot. `cli -s` asks for the
reload itself, and checks readiness before it does, so a new script should not add its own
`killall`.

Raise both against files under `general/overlay/` and anywhere under
`general/package/` — including nested layouts such as
`general/package/legacy/datalink/files/`.

The narrow, decidable half of each is a compliance gate ("cli writes address a real
setting, and reloads use SIGHUP" in `pr_compliance_checklist.yaml`): a literal path with a
stray character, and a reload asked for with something other than SIGHUP. What stays a
judgement call here is everything the diff cannot settle — whether a well-formed key is one
the target build actually declares, whether a path assembled at runtime is right, and
whether a script has a good reason to signal by hand. Lifecycle signalling is neither:
sysupgrade's SIGQUIT, and SIGTERM to stop the daemon, are not reload attempts and are not
in scope for either.

### 7.3 A shipped script may only call commands the image contains

A script under `general/overlay/` runs on a camera with no package manager, no
`$PATH` beyond what the rootfs holds, and frequently no network. A command that is not
there does not fail loudly: `sh` prints "not found" to a console nobody is reading and
carries straight on to the next line, so the script reports success having done nothing.

`ipctool` is the trap worth knowing by name, because a defconfig line looks like it
supplies it and does not. `general/package/ipctool/ipctool.mk` installs `ipcinfo` and
nothing else, so `BR2_PACKAGE_IPCTOOL=y` puts no binary called `ipctool` on the image.
What answers to that name is `/usr/sbin/ipctool`, a symlink to
`general/overlay/usr/sbin/extutils`, whose `ipctool)` arm curls the tool from
`https://github.com/OpenIPC/ipctool/releases/download/latest/` into `/tmp` the first
time somebody asks for it — "installed as remote GitHub plugin", as it says. That is a
debugging convenience for a person at a shell, and it is four bad properties in a boot
script: a network fetch during boot, from a floating `latest` tag, repeated every boot
because `/tmp` is tmpfs, and a silent no-op until the network is up.

`#2446` used `ipctool i2cset --bus 0 0x60 ...` from `rc.local`, twelve seconds into
boot, on a WiFi-only camera, to apply the sensor registers the whole change depends on.

Flag a call in `general/overlay/` or `general/package/*/files/` to anything that is not
a busybox applet and not installed by a package the board's defconfig selects. Reach for
`ipcinfo` where the information is what is wanted, and for the vendor `load_<vendor>`
script or the SDK driver where hardware must actually be programmed (§3.5).

### 7.4 Restart majestic through its init script, not by hand

`general/package/majestic/files/S95majestic` is not a thin wrapper. It re-reads `/etc/TZ`
so a restart picks up a zone change; it passes `-s`; it finds the daemon with
`start-stop-daemon -x` rather than a pidfile, because a stale pidfile makes `-S` start a
*second* majestic that dies on the busy sensor HAL; it waits up to ten seconds for the
old process to actually exit; and it starts the new one under `trap '' HUP`, because the
disposition survives `exec()` and a handler does not — the script's own comment records
25 out of 25 deaths without it, measured on a hi3516ev200.

A hand-rolled `killall majestic; sleep 3; majestic &` re-opens every one of those, which
is what `#2446` shipped, on a hi3516ev200.

Flag a shipped script that stops or starts majestic directly. `/etc/init.d/S95majestic
restart` does it correctly; `reload` is the SIGHUP path in §7.2. Lifecycle signalling by
other consumers — sysupgrade's SIGQUIT to make majestic release the SDK while staying
alive, or SIGTERM to stop it — is a different thing and is not in scope here.

---

## 8. Things that must not reach `master`

These are hard gates rather than judgement calls; `pr_compliance_checklist.yaml`
enforces them. Summarised here because they are the most common review findings:

- `LD_PRELOAD` in any shipped script, package file, or overlay.
- Binaries extracted from a camera's factory firmware, or anything the diff renders as
  `Binary files ... differ`, with no vendor SDK or buildable source behind it —
  extension and path do not matter, and `general/overlay/` is never the place for one.
- New kernel patches under `general/package/all-patches/linux/` — those go to
  OpenIPC/linux.
- A new `*.patch` against a package whose `*_SITE` is an `openipc` repository, unless it
  is a named, minimal, temporary bridge to a pull request already open there (§4.4).
- A `*_SITE` pointing at a personal fork, or a `*_VERSION` that is an abbreviated SHA.
- A sensor, GPIO, I2C address, or other board-specific value written into
  `general/overlay/` or into a shared `load_<vendor>` default.
- Single-board scripts and packages in the shared tree — those go to OpenIPC/builder,
  through the per-device seams in §1.4.
- New sources that no `Config.in` selects and no defconfig builds, and overlay files
  that nothing on the camera reads.
- A shipped script calling a command the image does not contain — `ipctool` is the one
  that looks installed and is not.
- `insmod` where the tree uses `modprobe`, an OpenSDK module not named `open_*`, or a
  hand-rolled majestic restart in place of `/etc/init.d/S95majestic restart`.
- A test plan whose boxes are unchecked, or a description stating the change was not
  tested on hardware.
