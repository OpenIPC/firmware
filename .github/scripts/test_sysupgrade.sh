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
    -e "s|/proc/sys/vm/drop_caches|@SB@/tmp/drop_caches|g" \
    -e "s|/etc/init.d/|@SB@/etc/init.d/|g" \
    -e "s|@OSRELEASE@|/etc/os-release|g" \
    -e "s|@SB@|$SB|g" \
    "$SRC" > "$SB/sysupgrade"

grep -q '@SB@\|@OSRELEASE@' "$SB/sysupgrade" \
    && { echo "FAIL sandbox rewrite left an unexpanded sentinel"; exit 1; }

for s in S99rc.local S60crond S49ntpd S02klogd S01syslogd; do
    printf '#!/bin/sh\nexit 0\n' > "$SB/etc/init.d/$s"; chmod +x "$SB/etc/init.d/$s"
done

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
        abort_wait=0 \
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
    unset STUB_FLASHCP_FAIL
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
echo "=== Part 2: invariants in $SRC ==="

# An option named in a user-facing message must exist in the parser.
for opt in $(grep -oE '\-\-[a-z_]+' "$SRC" | sort -u); do
    case "$opt" in
        --force_*|--wipe_overlay|--no_reboot|--no_update|--help|--web|--url|--archive|--kernel|--rootfs|--channel|--build|--list*|--connect*) continue ;;
    esac
    bad "message references '$opt', which the option parser does not accept"
done
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
