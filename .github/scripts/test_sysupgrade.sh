#!/bin/bash
# Regression tests for sysupgrade's rootfs verification.
#
# Catches the half-flash class of bug: do_update_rootfs() used to loop-mount the
# new rootfs to read its SoC stamp and version, but the kernel was flashed FIRST
# and the rootfs verified AFTER. A rootfs that failed verification was only ever
# discovered once the kernel had been committed, leaving a device with a new
# kernel on an old rootfs and no way back. The same mount could also block
# indefinitely (no output between the echo and the write, so a wedged mount was
# indistinguishable from a dead tool), and it died on images that flashcp — which
# writes the partition raw and never needs a mount — would have written fine.
#
# Two-part check:
#   Part 1 — behaviour: run the real sysupgrade against a stubbed device and
#            assert what reached the flash. Every failure case asserts that the
#            KERNEL WAS NEVER WRITTEN; that is the property that mattered.
#   Part 2 — drift: static assertions on the script itself, for invariants a
#            behaviour test cannot pin (e.g. an option a message advises must
#            actually exist in the parser).
#
# Lightweight: pure shell, no QEMU, no root, runs in a few seconds.

set -u

# Overridable so the suite can be pointed at an older sysupgrade to confirm it
# actually reproduces the bugs it claims to guard against.
SRC=${SRC:-general/overlay/usr/sbin/sysupgrade}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

[ -f "$SRC" ] || { echo "FAIL cannot find $SRC — run me from the repo root"; exit 1; }

for t in xxd od timeout dash; do
    command -v "$t" >/dev/null 2>&1 || { echo "FAIL required tool '$t' is missing"; exit 1; }
done

# ---------------------------------------------------------------------------
# Sandbox: the device paths sysupgrade hardcodes, redirected under $SB, plus
# stubs on PATH for everything that would touch real hardware.
# ---------------------------------------------------------------------------
SB=$(mktemp -d)
trap 'rm -rf "$SB"' EXIT
mkdir -p "$SB/tmp" "$SB/proc" "$SB/etc/init.d" "$SB/bin"

# Rewrite the absolute device paths sysupgrade hardcodes to point into $SB.
# /dev/* is deliberately NOT rewritten: flashcp is stubbed and records its argv,
# so the target only needs to be a recognisable string.
#
# Order matters, and paths are staged through sentinels rather than substituted
# inline: $SB itself lives under /tmp, so a naive `s|/tmp|$SB/tmp|` would go on
# to rewrite the very paths the earlier rules had just produced.
#
#  1. get_system_version() takes its root as "$1" — "" for the running system
#     (which must be sandboxed) and the mountpoint for the candidate rootfs
#     (which must NOT be). Give it a default and hide the literal behind a
#     sentinel so rule 3 cannot touch it. Getting this wrong makes every
#     version read empty, which silently turns the same-version test green.
#  2. /tmp\b, before anything that inserts a /tmp path of its own.
sed -e 's|grep "GITHUB_VERSION" "$1/etc/os-release"|grep "GITHUB_VERSION" "${1:-@SB@}@OSRELEASE@"|' \
    -e "s|/tmp\\b|@SB@/tmp|g" \
    -e "s|/etc/os-release|@SB@/etc/os-release|g" \
    -e "s|/proc/mtd|@SB@/proc/mtd|g" \
    -e "s|/proc/cmdline|@SB@/proc/cmdline|g" \
    -e "s|/proc/mounts|@SB@/proc/mounts|g" \
    -e "s|/proc/sys/vm/drop_caches|@SB@/tmp/drop_caches|g" \
    -e "s|/etc/init.d/|@SB@/etc/init.d/|g" \
    -e "s|/bin/busybox|@SB@/bin/busybox|g" \
    -e "s|@OSRELEASE@|/etc/os-release|g" \
    -e "s|@SB@|$SB|g" \
    "$SRC" > "$SB/sysupgrade"

grep -q '@SB@\|@OSRELEASE@' "$SB/sysupgrade" \
    && { echo "FAIL sandbox rewrite left an unexpanded sentinel"; exit 1; }

for s in S99rc.local S60crond S49ntpd S02klogd S01syslogd; do
    printf '#!/bin/sh\nexit 0\n' > "$SB/etc/init.d/$s"; chmod +x "$SB/etc/init.d/$s"
done
# This one logs: whether majestic is put back is a behaviour worth asserting,
# not just a service that had to exist for the script not to trip over it.
printf '#!/bin/sh\necho "S95majestic $1" >> "$FLASH_LOG"\nexit 0\n' \
    > "$SB/etc/init.d/S95majestic"; chmod +x "$SB/etc/init.d/S95majestic"

# ramfs_unwind verifies the restore by reading /proc/mounts before it lets the
# in-place fallback run, and enter_ramfs consults it to decide whether a leftover
# tmpfs of its own needs clearing. Give the sandbox one that says the mounts are
# where they should be, so the fallback cases exercise the fallback rather than
# the emergency "cannot restore, reboot" path. Deliberately no line for $SB/ram:
# a stale ramfs is the exception, not the default.
printf 'tmpfs %s/tmp tmpfs rw,relatime 0 0\nproc %s/proc proc rw,relatime 0 0\n' \
    "$SB" "$SB" > "$SB/proc/mounts"

set_mtd() { cat > "$SB/proc/mtd"; }

set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00200000 00010000 "kernel"
mtd3: 00500000 00010000 "rootfs"
mtd4: 00100000 00010000 "rootfs_data"
EOF

# The kernel command line decides whether this camera is running FROM the flash
# sysupgrade is about to rewrite. The default is the real one off the lab
# hi3516ev300 -- deliberately including mmz=anonymous and an mtdparts label
# ending in "rootfs_data", the substrings a loose root-type probe trips over.
set_cmdline() { printf '%s\n' "$1" > "$SB/proc/cmdline"; }
CMDLINE_FLASH='mem=128M console=ttyAMA0,115200 panic=20 rootfstype=squashfs root=/dev/mtdblock3 mtdparts=hi_sfc:256k(boot),64k(env),2048k(kernel),5120k(rootfs),-(rootfs_data) mmz_allocator=cma mmz=anonymous,0,0x42000000,96M init=/init'
CMDLINE_NFS='console=ttyAMA0,115200 root=/dev/nfs nfsroot=192.168.1.1:/srv/cam,tcp,v3 ip=dhcp rw'
CMDLINE_MMC='console=ttyAMA0,115200 root=/dev/mmcblk0p2 rootfstype=ext4 rw'
CMDLINE_RAM='console=ttyAMA0,115200 root=/dev/ram0 rdinit=/linuxrc'
set_cmdline "$CMDLINE_FLASH"

# --- stubs -----------------------------------------------------------------
stub() { printf '#!/bin/bash\n%s\n' "$2" > "$SB/bin/$1"; chmod +x "$SB/bin/$1"; }

stub ipcinfo    'case "$1" in -v) echo "${STUB_VENDOR:-sigmastar}";; -F) echo nor;; esac'
stub fw_printenv 'echo "${STUB_SOC:-ssc338q}"'
stub killall    'exit 0'
stub ntpd       'exit 0'
stub curl       'exit 0'
stub umount     'exit 0'
# Logs so ordering can be asserted, and fails by default: an unprivileged test
# host cannot really pivot, and the fallback is the safety property that matters
# most here (a camera that cannot build a ramfs must still upgrade).
stub pivot_root 'echo "pivot_root $*" >> "$FLASH_LOG"; exit ${STUB_PIVOT_RC:-1}'
stub losetup    'case "$1" in -f) echo /dev/loop0;; *) exit 0;; esac'

# download_firmware runs `md5sum -s -c`. -s (silent) is a busybox extension; GNU
# coreutils spells it --status and rejects -s outright. Bridge it, so the real
# checksum gate is exercised here rather than stubbed away.
REAL_MD5=$(command -v md5sum)
cat > "$SB/bin/md5sum" <<EOF
#!/bin/bash
a=(); s=0
for x in "\$@"; do
    case "\$x" in -s) s=1 ;; *) a+=("\$x") ;; esac
done
[ \$s -eq 1 ] && exec "$REAL_MD5" --status "\${a[@]}"
exec "$REAL_MD5" "\${a[@]}"
EOF
chmod +x "$SB/bin/md5sum"

# flashcp / flash_eraseall / reboot all arrive via `busybox <applet>`; record the
# argv of anything that writes, in order, so a test can assert both WHAT was
# written and WHETHER anything was.
#
# STUB_FLASHCP_FAIL makes the write fail AFTER it has been logged -- a partially
# erased partition, which is the state do_update_firmware's `|| die` reacts to.
stub busybox '
applet=$1; shift
case "$applet" in
    flashcp|flash_eraseall) echo "$applet $*" >> "$FLASH_LOG"
                            [ "1" = "$STUB_FLASHCP_FAIL" ] && exit 1 ;;
    reboot)                 echo "reboot" >> "$FLASH_LOG"; exit 0 ;;
esac
exit 0'

# The verify-mount. STUB_MOUNT picks the behaviour under test.
#   ok   — mount succeeds; populate the mountpoint like a real rootfs
#   fail — mount fails the way a missing squashfs decompressor does
#   hang — mount blocks; only a bounded caller survives this
# A bare `mount` (check_sdcard's `mount | grep /mnt/mmc`) must stay quiet.
stub mount '
[ $# -eq 0 ] && exit 0
target=${!#}
# STUB_MOUNT models the VERIFY-mount (a squashfs image over a loop device). The
# ramfs pivot mounts tmpfs and relocates existing mounts; those are a different
# operation and must not inherit the fault being injected, or the hang case
# wedges before the code under test is even reached.
for a in "$@"; do
    case "$a" in
        move|tmpfs|--move|-M) exit 0 ;;
    esac
done
case "${STUB_MOUNT:-ok}" in
    ok)
        mkdir -p "$target/etc"
        echo "GITHUB_VERSION=${STUB_IMG_VERSION:-2026.07.11}" > "$target/etc/os-release"
        echo "openipc-${STUB_IMG_SOC:-ssc338q}" > "$target/etc/hostname"
        exit 0 ;;
    fail)
        echo "mount: mounting /dev/loop0 on $target failed: Invalid argument" >&2
        exit 255 ;;
    hang)
        # exec, so a bounded caller TERMs the sleep itself rather than a wrapper
        # that leaves it orphaned. Far longer than mount_wait, so an unbounded
        # caller is unmistakable.
        exec sleep "${STUB_HANG_SECS:-600}" ;;
esac'

cat > "$SB/etc/os-release" <<'EOF'
BUILD_PLATFORM=ssc338q_lite
BUILD_OPTION=lite
GITHUB_VERSION=2026.06.01
BUILD_ID=nightly-20260601-aaaaaaa
EOF

# --- fixtures --------------------------------------------------------------
# A legacy uImage: 32-byte header (magic 0x27051956, timestamp at offset 8),
# then the name field do_update_kernel probes for the SoC via `od -j 32`.
make_uimage() {
    printf '\x27\x05\x19\x56' > "$1"          # 0..3   magic
    printf '\x00\x00\x00\x00' >> "$1"         # 4..7   hcrc
    printf '\x68\x8f\x00\x00' >> "$1"         # 8..11  timestamp
    dd if=/dev/zero bs=1 count=20 >> "$1" 2>/dev/null   # 12..31 rest of header
    printf 'Linux-5.10-%s' "$2" >> "$1"       # 32..   name -> `cut -d- -f3`
    dd if=/dev/zero bs=1 count=8 >> "$1" 2>/dev/null
}

# A FIT image: DTB magic, no uImage SoC field and no timestamp. This is the
# shape do_update_kernel skips the SoC probe for.
make_fit() {
    printf '\xd0\x0d\xfe\xed' > "$1"
    dd if=/dev/zero bs=1 count=60 >> "$1" 2>/dev/null
}

make_rootfs() { dd if=/dev/zero bs=1k count=8 of="$1" 2>/dev/null; }

# A combined image (cv6xx): the FIT and the rootfs squashfs in one blob, rootfs
# packed after the FIT at a 64K-aligned offset. do_update_firmware splits it on
# the FIT totalsize (header bytes 4..7, big-endian) — one 64K block here.
make_combined() {
    printf '\xd0\x0d\xfe\xed' > "$1"                        # FIT magic
    printf '\x00\x01\x00\x00' >> "$1"                       # totalsize 0x10000
    dd if=/dev/zero bs=1 count=65528 >> "$1" 2>/dev/null    # rest of the FIT
    dd if=/dev/zero bs=1k count=8 >> "$1" 2>/dev/null       # rootfs remainder
}

# Pack $1.. into a .tgz at $SB/tmp/fw.tgz the way download_firmware expects
# (artifact names + an md5sum manifest beside them).
make_archive() {
    local stage="$SB/stage"
    rm -rf "$stage"; mkdir -p "$stage"
    local names="" f
    for f in "$@"; do cp "$f" "$stage/"; names="$names $(basename "$f")"; done
    # List the names explicitly rather than globbing: the manifest must not end
    # up checksumming itself.
    (cd "$stage" && md5sum $names > openipc.md5sum)
    (cd "$stage" && tar cf - . | gzip > "$SB/tmp/fw.tgz")
}

# --- runner ----------------------------------------------------------------
# run <name> -- <args...>; sets $OUT (combined output) and $RC.
OUT=""; RC=0
run() {
    rm -f "$SB/tmp/sysupgrade.lock" "$SB/tmp/flash.log"
    : > "$SB/tmp/flash.log"
    OUT=$(cd "$SB" && env PATH="$SB/bin:$PATH" \
        HASERLVER=1 FLASH_LOG="$SB/tmp/flash.log" mount_wait="${MOUNT_WAIT:-3}" \
        abort_wait=0 RAM_ROOT="$SB/ram" \
        STUB_PIVOT_RC="${STUB_PIVOT_RC:-1}" \
        STUB_MOUNT="${STUB_MOUNT:-ok}" STUB_VENDOR="${STUB_VENDOR:-sigmastar}" \
        STUB_SOC="${STUB_SOC:-ssc338q}" \
        STUB_IMG_SOC="${STUB_IMG_SOC:-ssc338q}" \
        STUB_IMG_VERSION="${STUB_IMG_VERSION:-2026.07.11}" \
        STUB_FLASHCP_FAIL="${STUB_FLASHCP_FAIL:-0}" \
        sh "$SB/sysupgrade" "$@" 2>&1)
    RC=$?
}
flashed()      { grep -q "flashcp .*$1" "$SB/tmp/flash.log"; }
erased()       { grep -q "flash_eraseall .*$1" "$SB/tmp/flash.log"; }
nothing_wrote() { ! grep -q "flashcp" "$SB/tmp/flash.log"; }
# The busybox stub logs a bare "reboot" line, so whether the run rebooted is
# directly observable -- which is the whole question issue #2231 turns on.
rebooted()     { grep -qx "reboot" "$SB/tmp/flash.log"; }
# Line number of a phrase in $OUT, for ordering assertions.
at() { printf '%s\n' "$OUT" | grep -n -- "$1" | head -1 | cut -d: -f1; }

reset_env() {
    unset STUB_MOUNT STUB_VENDOR STUB_SOC STUB_IMG_SOC STUB_IMG_VERSION MOUNT_WAIT
    unset STUB_FLASHCP_FAIL STUB_PIVOT_RC
    rm -rf "$SB/ram"
    rm -f "$SB"/tmp/*.ssc338q "$SB"/tmp/firmware.bin.* "$SB"/tmp/*.tgz "$SB"/tmp/*.md5sum
    make_uimage "$SB/tmp/uImage.ssc338q" ssc338q
    make_rootfs "$SB/tmp/rootfs.squashfs.ssc338q"
    set_cmdline "$CMDLINE_FLASH"
    set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00200000 00010000 "kernel"
mtd3: 00500000 00010000 "rootfs"
mtd4: 00100000 00010000 "rootfs_data"
EOF
}

K="$SB/tmp/uImage.ssc338q"
R="$SB/tmp/rootfs.squashfs.ssc338q"

echo "=== Part 1: sysupgrade rootfs verification behaviour ==="

# --- the happy path --------------------------------------------------------
reset_env
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3; then
    ok "mount ok, new version -> kernel and rootfs both flashed"
else
    bad "mount ok, new version -> expected both flashed, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# The reason this PR exists: verification must precede the first write.
v=$(at "Verifying rootfs from"); k=$(at "Update kernel from")
if [ -n "$v" ] && [ -n "$k" ] && [ "$v" -lt "$k" ]; then
    ok "rootfs is verified BEFORE the kernel is flashed"
else
    bad "verify must precede the kernel flash -- verify@${v:-none} kernel@${k:-none}"
fi

# Assignments are made on their own line, never as an env-prefix to run(): a
# prefix on a *function* call persists in the caller under POSIX sh but not
# under default bash, and that difference is not worth depending on.
reset_env
STUB_IMG_VERSION=2026.06.01   # == the installed version
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && ! flashed /dev/mtd3; then
    ok "mount ok, same version -> rootfs write skipped"
else
    bad "same version -> rootfs should not be written, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

reset_env
STUB_IMG_SOC=gk7205v300   # a foreign image
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote; then
    ok "mount ok, wrong SoC -> refused, nothing written"
else
    bad "wrong SoC -> expected refusal with no write, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# --- unmountable rootfs ----------------------------------------------------
# Not a defect in the image: the RUNNING kernel lacks the decompressor the NEW
# image uses. flashcp never needs the mount, so this must not reject the flash.
reset_env
STUB_MOUNT=fail
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3; then
    ok "mount fails, kernel in same run -> warn and flash both"
else
    bad "mount fail + kernel -> expected both flashed, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
if printf '%s' "$OUT" | grep -q "Invalid argument"; then
    ok "mount failure surfaces the real mount error, not just a guess"
else
    bad "mount failure should print the underlying error; got: $(printf '%s' "$OUT" | tail -3)"
fi

# A wedged mount must not outlive the flash it guards.
reset_env
start=$(date +%s)
STUB_MOUNT=hang
MOUNT_WAIT=3
run -z --kernel="$K" --rootfs="$R"
elapsed=$(( $(date +%s) - start ))
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3 && [ "$elapsed" -lt 15 ]; then
    ok "mount hangs -> bounded by mount_wait (${elapsed}s), flash proceeds"
else
    bad "mount hang -> expected a run bounded near mount_wait=3s and both flashed, rc=$RC ${elapsed}s log='$(cat "$SB/tmp/flash.log")'"
fi

# No mount, and no kernel in the run to vouch for the SoC either: the one case
# worth refusing -- and it costs nothing, because nothing has been written.
reset_env
STUB_MOUNT=fail
run -z --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote; then
    ok "mount fails, rootfs-only -> refused, nothing written"
else
    bad "mount fail + rootfs-only -> expected refusal with no write, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
# The advice in that refusal has to be an option the parser actually accepts.
if printf '%s' "$OUT" | grep -q -- "--force_soc" && ! printf '%s' "$OUT" | grep -q -- "--skip_soc"; then
    ok "refusal advises --force_soc (a real option), not --skip_soc"
else
    bad "refusal must advise --force_soc; got: $(printf '%s' "$OUT" | grep -i 'pass --' | head -1)"
fi

reset_env
STUB_MOUNT=fail
run -z --force_soc --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3; then
    ok "mount fails, rootfs-only, --force_soc -> proceeds"
else
    bad "--force_soc should override the refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# A kernel in the run only vouches for the SoC if its header actually carries
# one. A FIT does not, and check_soc is skipped for ingenic/rockchip -- so these
# must be treated like rootfs-only, not waved through.
reset_env
make_fit "$SB/tmp/uImage.ssc338q"
STUB_MOUNT=fail
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote; then
    ok "mount fails, FIT kernel, local files -> refused (FIT carries no SoC)"
else
    bad "FIT kernel is no SoC witness -> expected refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

reset_env
STUB_MOUNT=fail
STUB_VENDOR=ingenic
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote; then
    ok "mount fails, ingenic, local files -> refused (check_soc is skipped there)"
else
    bad "ingenic kernel is no SoC witness -> expected refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# ...but a downloaded artifact is pinned to $model by the name it must have to
# be found at all, so the same evidence gap must not refuse a real upgrade.
reset_env
make_fit "$SB/tmp/uImage.ssc338q"
make_archive "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
STUB_MOUNT=fail
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3; then
    ok "mount fails, FIT kernel, downloaded archive -> proceeds (name-pinned)"
else
    bad "name-pinned archive should not be refused, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# --- the combined image (cv6xx: FIT + rootfs in one blob) -------------------
# do_update_firmware splits the blob and writes the two partitions separately.
# Master verified the rootfs only after the FIT had been committed, so this path
# carried the same half-flash as the split path -- unreported until review.
reset_env
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3; then
    ok "combined image, mount ok -> split, kernel and rootfs both flashed"
else
    bad "combined image -> expected both flashed, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
v=$(at "Verifying rootfs from"); k=$(at "Update kernel from")
if [ -n "$v" ] && [ -n "$k" ] && [ "$v" -lt "$k" ]; then
    ok "combined image: rootfs is verified BEFORE the FIT is flashed"
else
    bad "combined path must verify before the FIT write -- verify@${v:-none} kernel@${k:-none}"
fi

# The combined half-flash itself: a foreign image must cost nothing.
reset_env
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
STUB_IMG_SOC=gk7205v300
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_wrote; then
    ok "combined image, wrong SoC -> refused, FIT never written"
else
    bad "combined + wrong SoC -> expected refusal with no write, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# --- transcript ------------------------------------------------------------
reset_env
run -z --kernel="$K" --rootfs="$R"
if printf '%s' "$OUT" | grep -q "Verifying rootfs from"; then
    ok "verify announces itself (a stall has a last line to stop at)"
else
    bad "verify should print its own header/line before mounting"
fi

# --- -x / --no_reboot on a camera flashing its own live rootfs (issue #2231) --
#
# A NOR camera boots an overlay whose lowerdir is the squashfs on the "rootfs"
# MTD partition and whose upperdir is the jffs2 on "rootfs_data" -- the two
# partitions sysupgrade erases. free_resources() drops the page cache to make
# room for the download, so once flashcp has rewritten that partition every
# read that misses the cache comes back as the NEW image at STALE offsets: SSH
# auth, libc and /etc all return garbage. Honouring --no_reboot there does not
# leave a working camera pending a convenient reboot, it leaves a dead one that
# still answers ping and cannot be logged into to issue the reboot at all.
#
# So the reboot decision cannot rest on skip_reboot alone. It has to ask what
# actually got written, and whether this camera is running from it.

echo
echo "=== Part 1b: --no_reboot vs the live rootfs (issue #2231) ==="

# The bug itself. -x must not be honoured once the live rootfs is overwritten.
reset_env
run -z --rootfs="$R" -x
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && rebooted; then
    ok "-x + live rootfs rewritten -> reboots anyway"
else
    bad "-x + live rootfs -> expected flash then reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
# And it must say why, rather than rebooting a camera whose operator asked it not to.
if printf '%s' "$OUT" | grep -q -- "--no_reboot ignored"; then
    ok "-x override explains itself"
else
    bad "-x override must explain itself; got: $(printf '%s' "$OUT" | tail -3)"
fi
# The warning has to come BEFORE the write, while Ctrl-C still works: the trap
# that shields flashcp from a dying TTY also takes the operator's way out.
n=$(at "NOTICE"); w=$(at "Update rootfs from")
if [ -n "$n" ] && [ -n "$w" ] && [ "$n" -lt "$w" ]; then
    ok "-x notice precedes the first write (abort window is real)"
else
    bad "-x notice must precede the write -- notice@${n:-none} write@${w:-none}"
fi

# The flag still has a job. A kernel-only run never touches the mounted
# partition, so the camera survives it and -x means what it says.
reset_env
run -z --kernel="$K" -x
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && ! flashed /dev/mtd3 && ! rebooted; then
    ok "-x + kernel only -> honoured, no reboot"
else
    bad "-x + kernel only -> expected no reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
# ...and exits 0. A deliberate --no_reboot used to report failure.
if printf '%s' "$OUT" | grep -q "asked me not to reboot"; then
    ok "-x honoured path prints the soft notice and exits $RC"
else
    bad "-x honoured path should print the soft notice; got: $(printf '%s' "$OUT" | tail -3)"
fi

# Same version: do_update_rootfs returns before flashcp, nothing is written, so
# there is nothing to reboot for. Deciding on intent rather than on what was
# written would get this wrong.
reset_env
STUB_IMG_VERSION=2026.06.01
run -z --rootfs="$R" -x
if [ "$RC" -eq 0 ] && ! flashed /dev/mtd3 && ! rebooted; then
    ok "-x + same version -> nothing written, honoured"
else
    bad "-x + same version -> expected no write and no reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# Not every camera runs from the flash it writes. An NFS/SD/ram root is a
# supported layout (general/overlay/init, general/package/openipc-nfs-root)
# where the rootfs partition is just a target and -x is exactly the right flag.
for c in "$CMDLINE_NFS:nfs" "$CMDLINE_MMC:mmcblk" "$CMDLINE_RAM:ram"; do
    reset_env
    set_cmdline "${c%:*}"
    run -z --rootfs="$R" -x
    if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && ! rebooted; then
        ok "-x + ${c##*:} root -> rootfs flashed, honoured (not running from it)"
    else
        bad "-x + ${c##*:} root -> expected flash with no reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
    fi
done

# The failure that would brick a camera is a FALSE "not on flash", so the probe
# is anchored to the root= token. The stock hi3516ev300 command line carries
# both `mmz=anonymous` and an mtdparts label ending `-(rootfs_data)`; a probe
# matching a bare `ram`/`mmcblk` anywhere in the line is one vendor bootarg away
# from waving through the exact case this test exists for.
reset_env
set_cmdline "$CMDLINE_FLASH ramdisk_size=8192 mmz=mmcblkish"
run -z --rootfs="$R" -x
if flashed /dev/mtd3 && rebooted; then
    ok "-x + decoy 'ram'/'mmcblk' substrings -> still recognised as flash root"
else
    bad "root-type probe must anchor to root=; decoy substrings fooled it, log='$(cat "$SB/tmp/flash.log")'"
fi

# The combined image (cv6xx) reaches the rootfs by both of its paths.
reset_env
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
run -z --archive="$SB/tmp/fw.tgz" -x
if flashed /dev/mtd2 && flashed /dev/mtd3 && rebooted; then
    ok "-x + combined image, split path -> reboots anyway"
else
    bad "-x + combined split -> expected both flashed and a reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# No separate kernel/rootfs partitions: one whole-blob write to "firmware",
# which overlaps the running rootfs.
reset_env
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00700000 00010000 "firmware"
EOF
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
run -z --archive="$SB/tmp/fw.tgz" -x
if flashed /dev/mtd2 && rebooted; then
    ok "-x + combined image, whole-blob path -> reboots anyway"
else
    bad "-x + combined whole-blob -> expected flash and reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# A write that FAILS is not a write that did not happen: flashcp erases before
# it writes, so a partition left half-erased is every bit as unreadable. This
# is why the run is marked dirty before the write, not after -- do_update_firmware
# reaches reboot_system through `|| die` precisely here.
reset_env
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00700000 00010000 "firmware"
EOF
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
STUB_FLASHCP_FAIL=1
run -z --archive="$SB/tmp/fw.tgz" -x
if [ "$RC" -ne 0 ] && rebooted; then
    ok "-x + write started then failed -> reboots anyway (partial erase is fatal too)"
else
    bad "-x + failed write -> expected a reboot despite the failure, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# --wipe_overlay erases "rootfs_data" -- the rw jffs2 that is the upperdir of
# the running overlay, so overlayfs consults it for every lookup. Erasing it
# live breaks the camera just as thoroughly as rewriting the lowerdir.
reset_env
run -z --wipe_overlay -x
if erased /dev/mtd4 && rebooted; then
    ok "-x + --wipe_overlay on flash root -> reboots anyway (live upperdir erased)"
else
    bad "-x + --wipe_overlay -> expected erase and reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
# On a non-flash root init mounts a tmpfs overlay instead, so there is no live
# upperdir on the partition being erased.
reset_env
set_cmdline "$CMDLINE_NFS"
run -z --wipe_overlay -x
if [ "$RC" -eq 0 ] && erased /dev/mtd4 && ! rebooted; then
    ok "-x + --wipe_overlay on nfs root -> honoured"
else
    bad "-x + --wipe_overlay on nfs root -> expected no reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# Nothing was written, so a failure before the first flash still honours -x.
reset_env
STUB_IMG_SOC=gk7205v300
run -z --rootfs="$R" -x
if [ "$RC" -ne 0 ] && nothing_wrote && ! rebooted; then
    ok "-x + refusal before any write -> honoured, nothing written"
else
    bad "-x + pre-write refusal -> expected no write and no reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# ---------------------------------------------------------------------------
echo
echo "=== Part 1c: die() before the first write (majestic-webui issue #120) ==="

# The headline case. Without -x, a failure before anything reached flash used to
# reboot anyway, because die() called reboot_system unconditionally. Over
# /ws/upgrade that reboot is the ONLY thing the WebUI can observe, so it read a
# refused upgrade as a successful one and sent the user to a status page showing
# the version they already had.
reset_env
STUB_IMG_SOC=gk7205v300
run -z --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote && ! rebooted; then
    ok "pre-write refusal -> no reboot (nothing changed, so nothing to reboot into)"
else
    bad "pre-write refusal -> expected no write and no reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# ...and it must say so, because that string is what the WebUI watches for to
# stop waiting for a reboot that is never coming.
if printf '%s' "$OUT" | grep -q "Aborting\."; then
    ok "pre-write refusal still prints 'Aborting.'"
else
    bad "pre-write refusal must print 'Aborting.' so a GUI can tell it apart from a flash"
fi

# The reboot used to double as lock cleanup: /tmp went with it. Now that we stay
# up, die() has to release the lock itself or the next attempt is refused.
if [ ! -f "$SB/tmp/sysupgrade.lock" ]; then
    ok "pre-write refusal releases the lock (a retry is possible)"
else
    bad "pre-write refusal left $SB/tmp/sysupgrade.lock behind; the next run would be refused"
fi

# The other half, and the case the fix must not regress: once flashcp has
# started, the partition is erased whether or not the write finished, so a die()
# from there must still reboot. The combined-image path is the one that actually
# reaches die() after a write (`flashcp ... || die`); on the split path a failed
# flashcp is not fatal at all, which is why this uses a combined image.
# free_resources() stops syslog/klogd/ntpd/cron before the download, because
# dropping the page cache is how a small camera finds the RAM to unpack the
# image. The reboot used to hide that -- everything came back on the way up.
# Now that a pre-flash failure stays up, it has to be undone explicitly, or a
# refused upgrade quietly costs the user their logging and their cron jobs.
reset_env
STUB_IMG_SOC=gk7205v300
run -z --rootfs="$R"
if printf '%s' "$OUT" | grep -q "Restarting the services stopped for the upgrade"; then
    ok "pre-write refusal restarts what free_resources stopped"
else
    bad "pre-write refusal left services down; the camera stays up but degraded"
fi

# majestic must be RESTARTED, not started. free_resources' `killall -3` is not a
# terminate -- SIGQUIT makes majestic release the SDK and keep serving -- so it
# is still running, just with no video pipeline and no way back in place. Under
# --web it is worse: majestic latched itself into upgrade mode and never clears
# the flag.
if grep -q "S95majestic restart" "$SB/tmp/flash.log"; then
    ok "pre-write refusal restarts majestic (a gutted daemon needs more than start)"
else
    bad "majestic left running without its SDK; video stays down until a power cycle"
fi

# A lock this run did not take belongs to somebody else. die() is reachable
# before create_lock, so removing the lock unconditionally there would unlink a
# CONCURRENT sysupgrade's lock and let two upgrades run at once. run() clears
# the lock first, so plant it afterwards and drive the script directly.
reset_env
: > "$SB/tmp/sysupgrade.lock"
OUT=$(cd "$SB" && env PATH="$SB/bin:$PATH" HASERLVER=1 \
    FLASH_LOG="$SB/tmp/flash.log" abort_wait=0 STUB_IMG_SOC=gk7205v300 \
    sh "$SB/sysupgrade" -z --rootfs="$R" 2>&1)
RC=$?
if [ -f "$SB/tmp/sysupgrade.lock" ] && [ "$RC" -ne 0 ]; then
    ok "a lock this run did not create is left alone"
else
    bad "a foreign lock was removed (rc=$RC); mutual exclusion is defeated"
fi
rm -f "$SB/tmp/sysupgrade.lock"

# The whole-blob layout is the one that reaches die() after a write
# (`flashcp ... || die`); on the split path a failed flashcp is not fatal at all.
# Same setup as the -x case above, minus the -x: the default path must reboot
# from here for the same reason, and this is what the fix must not regress.
reset_env
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00700000 00010000 "firmware"
EOF
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
STUB_FLASHCP_FAIL=1
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && rebooted; then
    ok "die() after a write started -> still reboots (partial erase is not survivable)"
else
    bad "post-write die -> expected a reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# ---------------------------------------------------------------------------
echo
echo "=== Part 1d: the default path is untouched by all of the above ==="

# The default path must be untouched by all of the above.
reset_env
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3 && rebooted; then
    ok "no -x -> unconditional reboot, unchanged"
else
    bad "no -x -> expected both flashed and a reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
# ...and it must not print the -x notice at people who never passed -x.
if ! printf '%s' "$OUT" | grep -q "NOTICE\|--no_reboot ignored"; then
    ok "no -x -> no --no_reboot chatter"
else
    bad "a run without -x should not mention --no_reboot"
fi

# ---------------------------------------------------------------------------
echo
echo "=== Part 1e: flashing from a ramfs (majestic-webui issue #120) ==="

# flashcp rewrites the partition backing the live squashfs, so from the first
# write onwards any rootfs page that is not resident reads back as the new image
# at a stale offset. The reboot is a fork+exec of busybox FROM that partition, so
# on a small camera it fails with EIO and the box never reboots -- it just stops,
# needing a power cycle. Everything after the first write therefore has to run
# from RAM.
reset_env
run -z --kernel="$K" --rootfs="$R"
pline=$(grep -n "pivot_root" "$SB/tmp/flash.log" | head -1 | cut -d: -f1)
fline=$(grep -n "flashcp"    "$SB/tmp/flash.log" | head -1 | cut -d: -f1)
if [ -n "$pline" ] && [ -n "$fline" ] && [ "$pline" -lt "$fline" ]; then
    ok "the ramfs pivot is attempted before the first write"
else
    bad "pivot must be attempted before any write (pivot=$pline first-write=$fline)"
fi

# ...but a camera that cannot build one must still upgrade. The pivot_root stub
# fails by default, so this run took the fallback.
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3 && rebooted; then
    ok "a failed pivot falls back to flashing in place"
else
    bad "failed pivot -> expected both flashed and a reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# The escape hatch for board bring-up.
reset_env
run -z --kernel="$K" --rootfs="$R" --no_ramfs
if ! grep -q "pivot_root" "$SB/tmp/flash.log" && flashed /dev/mtd3 && rebooted; then
    ok "--no_ramfs skips the pivot and flashes in place"
else
    bad "--no_ramfs -> expected no pivot but a normal flash, log='$(cat "$SB/tmp/flash.log")'"
fi

# A pivot that did not happen must still hand the old root back as it found it.
# The shipped rootfs has no /ram, so the directory enter_ramfs makes lands in the
# overlay's upper layer and stays there: every upgrade since the pivot landed
# left a stray /overlay/root/ram behind (OpenIPC/majestic-webui#120).
#
# rmdir is stubbed rather than left real because this sandbox fakes `mount` --
# nothing was ever mounted on $SB/ram, so it still holds the staged busybox tree
# and a real rmdir would rightly refuse to remove it.
stub rmdir 'echo "rmdir $*" >> "$FLASH_LOG"; exit 0'
reset_env
run -z --kernel="$K" --rootfs="$R"
if grep -q "^rmdir .*/ram$" "$SB/tmp/flash.log"; then
    ok "a pivot that failed reclaims its ramfs mount point"
else
    bad "failed pivot left RAM_ROOT behind, log='$(cat "$SB/tmp/flash.log")'"
fi
rm -f "$SB/bin/rmdir"

# ---------------------------------------------------------------------------
echo
echo "=== Part 2: invariants in $SRC ==="

# An option named in a user-facing message must exist in the parser.
# --connect-timeout and --speed-limit/--speed-time are curl's, not ours.
for opt in $(grep -oE '\-\-[a-z_]+' "$SRC" | sort -u); do
    case "$opt" in
        --force_*|--wipe_overlay|--no_reboot|--no_update|--no_ramfs|--help|--web|--url|--archive|--kernel|--rootfs|--channel|--build|--list*|--connect*|--speed*) continue ;;
    esac
    bad "message references '$opt', which the option parser does not accept"
done
# die() must weigh whether flash was touched, not reboot on reflex. Checked in
# the source as well as behaviourally, because the behavioural cases can only
# reach a handful of the ~20 die() call sites.
awk '/^die\(\)/,/^}/' "$SRC" | grep -q 'flash_touched' \
    && ok "die() gates the reboot on whether flash was touched" \
    || bad "die() reboots unconditionally again -- a refused upgrade will read as a successful one"
awk '/^die\(\)/,/^}/' "$SRC" | grep -q 'lock_owned.*rm -f \$LOCK_FILE' \
    && ok "die() releases the lock, but only one this run owns" \
    || bad "die() must remove \$LOCK_FILE when it does not reboot -- and only if lock_owned"
awk '/^create_lock\(\)/,/^}/' "$SRC" | grep -q 'lock_owned=1' \
    && ok "create_lock records ownership" \
    || bad "create_lock must set lock_owned, or die() can never release its own lock"
awk '/^die\(\)/,/^}/' "$SRC" | grep -q 'restore_resources' \
    && ok "die() restarts the services free_resources stopped" \
    || bad "a pre-flash die() leaves syslog/klogd/ntpd/cron stopped on a camera that stays up"
awk '/^restore_resources\(\)/,/^}/' "$SRC" | grep -q 'S95majestic restart' \
    && ok "restore_resources restarts majestic rather than starting it" \
    || bad "SIGQUIT leaves majestic running without an SDK; 'start' is a no-op, it needs 'restart'"
grep -q 'mark_flash_touched' "$SRC" \
    && ok "the flash-touched marker exists" \
    || bad "mark_flash_touched is gone; die() cannot tell a pre-write failure apart"
awk '/^do_update_kernel\(\)/,/^}/' "$SRC" | grep -q 'mark_flash_touched' \
    && ok "do_update_kernel marks flash touched (not live-dirty, but still final)" \
    || bad "do_update_kernel must mark flash touched before its write"

# The download is bounded by throughput, not by total elapsed time: -m 120 was a
# disguised 60 KB/s floor that no slow camera could meet (majestic-webui #120).
grep -q -- '--speed-limit' "$SRC" \
    && ok "the download gives up on a stalled transfer, not on a slow one" \
    || bad "the download must use --speed-limit/--speed-time, not a total-time cap"
grep -qE 'curl[^|]*-m 120' "$SRC" \
    && bad "-m 120 is back: any link under ~60 KB/s can never finish a ~7 MB image" \
    || ok "no total-time cap tight enough to fail a slow link"

# A pivot without a re-exec is theatre: pivot_root changes what paths resolve to,
# but the running shell keeps its text mapped from the old squashfs inode, which
# is the very thing that has to stop being true.
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q '^\s*exec ' \
    && ok "enter_ramfs re-execs (pivot alone leaves the shell on the old rootfs)" \
    || bad "enter_ramfs must exec after pivoting, or the shell stays mapped to the flash"
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'chroot' \
    && ok "enter_ramfs chroots (pivot_root alone does not move this process's root)" \
    || bad "enter_ramfs must chroot after pivot_root"
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'ldd ' \
    && ok "enter_ramfs stages the shared libraries busybox needs" \
    || bad "busybox is dynamically linked; staging it without its libs gives an unrunnable ramfs"
for m in /dev /proc /tmp; do
    awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q "mount -o move $m" \
        && ok "enter_ramfs carries $m into the new root" \
        || bad "enter_ramfs must move $m across, or the flash phase loses it"
done
case "$(grep -m1 '^RAM_ROOT=' "$SRC")" in
    *'/tmp'*) bad "RAM_ROOT must not live under /tmp -- /tmp is moved into it" ;;
    *)        ok  "RAM_ROOT is outside /tmp" ;;
esac
grep -q '_ramfs_phase' "$SRC" \
    && ok "the second phase has an entry point" \
    || bad "the re-exec'd process has no way to skip to the flash"
# busybox dispatches on argv[0]; without symlinks the new root has no tools.
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'ln -sf busybox' \
    && ok "enter_ramfs installs busybox applet symlinks" \
    || bad "without applet symlinks the pivoted root cannot run od/cut/grep/flashcp"
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'for applet in sh flashcp reboot' \
    && ok "enter_ramfs refuses to pivot into a root missing the essentials" \
    || bad "enter_ramfs must verify the staged root can actually flash and reboot"
# The fallback is only a fallback if the mounts it needs are still where it left
# them. Every failure after the first `mount -o move` has to put them back, or a
# failed pivot strands the running camera with no /dev at all.
moved=$(awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -c 'mount -o move /')
unwound=$(awk '/^ramfs_unwind\(\)/,/^}/' "$SRC" | grep -c 'mount -o move "\$RAM_ROOT')
[ "$moved" -gt 0 ] && [ "$unwound" -ge "$moved" ] \
    && ok "ramfs_unwind restores every mount enter_ramfs moves ($unwound >= $moved)" \
    || bad "moves=$moved but only $unwound are restored; a failed pivot would leave them relocated"
bare=$(awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | awk '/mount -o move \/dev/,0' | grep -c 'return 1' )
guarded=$(awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | awk '/mount -o move \/dev/,0' | grep -c 'ramfs_unwind')
[ "$guarded" -ge 1 ] && [ "$bare" -le "$guarded" ] \
    && ok "no unguarded return between the mount moves and the pivot" \
    || bad "$bare return(s) after the moves but only $guarded unwind(s)"
# A shell redirection CREATES its target. Between the /dev move and the pivot the
# name /dev/null resolves inside the OLD root, which has no such node -- devtmpfs
# supplied it, and devtmpfs has just been moved away. So `2>/dev/null` there does
# not discard the error, it writes a regular file named `null` into that root.
# The root is an overlay, so the file lands in the upper layer and outlives the
# upgrade: cameras on four SoCs came back from a clean flash carrying a 0600
# /overlay/root/dev/null (OpenIPC/majestic-webui#120).
window=$(awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | awk '/mount -o move \/dev/,/pivot_root/')
printf '%s\n' "$window" | grep -q '>/dev/null' \
    && bad "enter_ramfs names /dev/null after moving /dev away -- that CREATES the file" \
    || ok "nothing between the /dev move and the pivot names /dev/null"
printf '%s\n' "$window" | grep -q '2>&3' \
    && ok "the window between the /dev move and the pivot redirects to fd 3" \
    || bad "post-move redirections must name a descriptor, not a path"
# ramfs_unwind is reached from inside that same window -- putting /dev back is
# what it exists for -- so it cannot name /dev/null either.
awk '/^ramfs_unwind\(\)/,/^}/' "$SRC" | grep -q '>/dev/null' \
    && bad "ramfs_unwind names /dev/null while /dev may still be moved away" \
    || ok "ramfs_unwind never names /dev/null in a redirection"
# ...and the descriptor only means anything if it was opened while /dev was still
# the devtmpfs, i.e. at the top level, before enter_ramfs can run.
grep -qE '^[^[:space:]].*exec 3>' "$SRC" \
    && ok "fd 3 is opened at the top level, while /dev is still the devtmpfs" \
    || bad "fd 3 must be opened before enter_ramfs starts relocating /dev"
# The mount point is the other half of the same leak: the shipped rootfs has no
# /ram, so `mkdir -p "$RAM_ROOT"` writes a directory into the overlay's upper
# layer, and nothing used to take it back out.
awk '/^ramfs_discard\(\)/,/^}/' "$SRC" | grep -q 'rmdir' \
    && ok "ramfs_discard reclaims the mount point enter_ramfs created" \
    || bad "nothing removes RAM_ROOT; the overlay gains an empty /ram per upgrade"
awk '/^ramfs_discard\(\)/,/^}/' "$SRC" | grep -q 'rm -rf' \
    && bad "ramfs_discard must not recursively delete a root the flash may still run from" \
    || ok "ramfs_discard removes an empty directory only (rmdir, never rm -rf)"
awk '/^ramfs_discard\(\)/,/^}/' "$SRC" | grep -qF '^tmpfs $RAM_ROOT tmpfs' \
    && ok "ramfs_discard only detaches a tmpfs of its own" \
    || bad "RAM_ROOT is overridable; an unconditional umount could detach real storage"
grep -qF 'rmdir "/mnt$RAM_ROOT"' "$SRC" \
    && ok "the ramfs phase reclaims the mount point stranded in the old root" \
    || bad "after the pivot nothing removes the old root's RAM_ROOT"
# ...and that "/mnt$RAM_ROOT" only names the right directory if RAM_ROOT is
# absolute. It is overridable from the environment, and a relative one would
# also defeat the /proc/mounts check, which records mount points absolutely.
grep -qE '^case "\$RAM_ROOT" in /\*\)' "$SRC" \
    && ok "a relative RAM_ROOT override is normalised to an absolute path" \
    || bad "RAM_ROOT is used as \"/mnt\$RAM_ROOT\" and matched against /proc/mounts; it must be absolute"
# That rmdir runs AFTER the pivot, where the applet symlinks are the only tools
# there are. `busybox --list` is a build option, so on a camera without it the
# hardcoded fallback list is the whole toolbox -- an applet missing from it is
# simply "not found", and a cleanup that fails is one that silently did nothing.
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | awk '/for applet in sh ash/,/done/' | grep -qw 'rmdir' \
    && ok "the fallback applet list stages rmdir for the ramfs phase" \
    || bad "the ramfs phase runs rmdir, but a busybox without --list would not have that applet"
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'export remote_update' \
    && ok "remote_update survives the re-exec (verify_rootfs branches on it)" \
    || bad "remote_update is not exported; an unmountable rootfs behaves differently in phase 2"
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'lock_owned' \
    && ok "lock_owned survives the re-exec" \
    || bad "lock_owned is not exported; die() in phase 2 cannot release its own lock"
# Restoring the mounts is attempted, not guaranteed. Handing the in-place path an
# environment with no /dev or /proc is exactly what ramfs_unwind exists to stop.
awk '/^ramfs_unwind\(\)/,/^}/' "$SRC" | grep -q '/dev/null.*||.*/proc/mounts\|! -c /dev/null' \
    && ok "ramfs_unwind checks the restore took before allowing the fallback" \
    || bad "ramfs_unwind returns without verifying /dev, /proc and /tmp came back"
awk '/^ramfs_unwind\(\)/,/^}/' "$SRC" | grep -q 'reboot -d 1 -f' \
    && ok "an unrestorable environment reboots rather than flashing blind" \
    || bad "if the restore fails there must be no in-place flash"
# RAM_ROOT is environment-overridable, so the pre-mount cleanup must not be a
# blind umount of whatever it happens to point at.
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'grep -q "\^tmpfs \$RAM_ROOT tmpfs"' \
    && ok "the pre-mount cleanup only clears a tmpfs of our own" \
    || bad "enter_ramfs unmounts \$RAM_ROOT blindly; pointed at real storage that detaches it"
# After the pivot the old system is behind /mnt, so exiting without a reboot
# leaves exactly the corpse this change exists to prevent.
awk '/^die\(\)/,/^}/' "$SRC" | grep -q '_ramfs_phase' \
    && ok "die() reboots unconditionally once we are in the ramfs" \
    || bad "a die() inside the ramfs must reboot; there is no system left to return to"
# The WebUI keeps majestic alive on purpose: it is the server streaming the log,
# and SIGQUIT to a majestic already in upgrade mode is a use-after-free
# (tracked daemon-side). The only legitimate one left is free_resources'
# non---web kill, which frees video memory for the download.
if [ "$(grep -c 'killall -q -3 majestic' "$SRC")" = "1" ] &&
    awk '/^free_resources\(\)/,/^}/' "$SRC" | grep -q 'killall -q -3 majestic'; then
    ok "the only SIGQUIT at majestic is free_resources' non---web one"
else
    bad "a SIGQUIT at majestic outside free_resources -- in upgrade mode that is a use-after-free"
fi
grep -q "Stopping web server before flashing" "$SRC" \
    && bad "the pre-flash 'stop the web server' step is back; --web exists to keep it serving" \
    || ok "no pre-flash stop of the web server"

grep -q -- '--skip_soc' "$SRC" \
    && bad "'--skip_soc' is not an option (the parser takes --force_soc)" \
    || ok "no reference to the non-existent --skip_soc"

# The ordering this PR is about: in the split path verify must be called before
# the kernel write, not from inside do_update_rootfs.
vline=$(grep -n '"\$update_rootfs" \] && verify_rootfs' "$SRC" | head -1 | cut -d: -f1)
kline=$(grep -n '"\$update_kernel" \] && do_update_kernel' "$SRC" | head -1 | cut -d: -f1)
if [ -n "$vline" ] && [ -n "$kline" ] && [ "$vline" -lt "$kline" ]; then
    ok "split path calls verify_rootfs before do_update_kernel"
else
    bad "split path must verify before flashing the kernel -- verify@${vline:-none} kernel@${kline:-none}"
fi

# The verification must not creep back into the write path.
if sed -n '/^do_update_rootfs()/,/^}/p' "$SRC" | grep -qE '\bmount|losetup'; then
    bad "do_update_rootfs mounts again -- verification belongs before the first write"
else
    ok "do_update_rootfs does not mount (it only writes)"
fi

# The bounded mount needs its fallback: CONFIG_TIMEOUT is set in busybox.config
# but not in busybox-initramfs.config.
if sed -n '/^mount_rootfs()/,/^}/p' "$SRC" | grep -q 'command -v timeout'; then
    ok "mount_rootfs falls back when the timeout applet is absent"
else
    bad "mount_rootfs must tolerate a busybox built without CONFIG_TIMEOUT"
fi

if grep -q '^CONFIG_TIMEOUT=y' general/package/busybox/busybox.config; then
    ok "busybox.config still provides the timeout applet"
else
    bad "CONFIG_TIMEOUT was dropped from busybox.config -- the bounded mount degrades"
fi

# --- issue #2231 invariants ------------------------------------------------

# skip_reboot must never be the sole gate again. The behaviour tests above only
# see the cases they were written for; this pins the shape.
rb=$(sed -n '/^reboot_system()/,/^}/p' "$SRC")
if printf '%s' "$rb" | grep -q 'live_flash_dirty'; then
    ok "reboot_system weighs what was written, not just skip_reboot"
else
    bad "reboot_system must consult live_flash_dirty, not skip_reboot alone"
fi

# It also must not exit on the honoured path: die() and the main flow disagree
# about the status, and the main flow's `exit 0` is what makes -x a success.
# Comments are stripped -- the ones in there discuss exit codes at length.
rbc=$(printf '%s\n' "$rb" | sed 's/#.*//')
if printf '%s\n' "$rbc" | grep -q 'return 0' && ! printf '%s\n' "$rbc" | grep -qw 'exit'; then
    ok "reboot_system returns rather than exits (caller owns the status)"
else
    bad "reboot_system must return on the honoured path so die() keeps its own exit 1"
fi

# Every write that lands on flash the camera is running from has to be marked,
# and the one that does not must stay unmarked or -x loses its only real use.
for fn in do_update_rootfs do_update_firmware do_wipe_overlay; do
    if sed -n "/^${fn}()/,/^}/p" "$SRC" | grep -q 'mark_live_flash_dirty'; then
        ok "$fn marks the live flash dirty"
    else
        bad "$fn writes flash the camera runs from and must mark it dirty"
    fi
done
if sed -n '/^do_update_kernel()/,/^}/p' "$SRC" | grep -q 'mark_live_flash_dirty'; then
    bad "do_update_kernel must NOT mark dirty -- the kernel partition is not mounted"
else
    ok "do_update_kernel leaves -x alone (its partition is not mounted)"
fi

# The mark belongs before the write (a half-erased partition is just as dead)
# and after the same-version return (which writes nothing at all).
body=$(sed -n '/^do_update_rootfs()/,/^}/p' "$SRC")
e=$(printf '%s\n' "$body" | grep -n 'exit_update'          | head -1 | cut -d: -f1)
m=$(printf '%s\n' "$body" | grep -n 'mark_live_flash_dirty' | head -1 | cut -d: -f1)
f=$(printf '%s\n' "$body" | grep -n 'flashcp'               | head -1 | cut -d: -f1)
if [ -n "$e" ] && [ -n "$m" ] && [ -n "$f" ] && [ "$e" -lt "$m" ] && [ "$m" -lt "$f" ]; then
    ok "do_update_rootfs marks after the same-version return and before the write"
else
    bad "do_update_rootfs order must be exit_update -> mark -> flashcp; got ${e:-none}/${m:-none}/${f:-none}"
fi

# The root-type probe must anchor to root=. An unanchored alternation matches a
# bare 'ram'/'mmcblk' anywhere in the command line, and a false "not on flash"
# is the one error here that bricks a camera.
if grep -q 'root_on_flash' "$SRC" \
   && grep -qF 'root=(/dev/)?(nfs|mmcblk|ram)' "$SRC" \
   && ! grep -qF 'nfs\|mmcblk\|ram' "$SRC"; then
    ok "root-type probe is anchored to the root= token"
else
    bad "root-type probe must anchor each alternative to root=, not match bare substrings"
fi

# The abort window has to stay overridable, or this suite pays for it six times.
if grep -q 'abort_wait=${abort_wait:-' "$SRC"; then
    ok "the -x abort window is overridable (abort_wait)"
else
    bad "abort_wait must stay overridable so the suite does not sleep through it"
fi

# The caveat belongs in --help, not only in the code.
if sed -n '/-x, --no_reboot/,/-z, --no_update/p' "$SRC" | grep -qi 'ignored'; then
    ok "--help says -x is ignored when the live flash is rewritten"
else
    bad "-x usage text must document that it is ignored on a live-flash rewrite"
fi

echo
if [ "$fail" -eq 0 ]; then
    echo "All sysupgrade verification checks passed."
    exit 0
else
    echo "$fail check(s) failed."
    exit 1
fi
