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
    -e "s|/proc/meminfo|@SB@/proc/meminfo|g" \
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
#
# The jffs2 line is what quiesce_overlay looks itself up in: it maps the
# rootfs_data partition get_device() reports to the mount point to remount
# read-only. set_mounts drops it for the "overlay is not mounted at all" case
# (NAND/UBI, or a root that never mounted one).
set_mounts() {
    printf 'tmpfs %s/tmp tmpfs rw,relatime 0 0\nproc %s/proc proc rw,relatime 0 0\n' \
        "$SB" "$SB" > "$SB/proc/mounts"
    case "${1:-}" in
        no-overlay) ;;
        # What general/overlay/init writes on a NAND camera: the same partition,
        # spelled as a UBI volume rather than an mtdblock device.
        ubi) printf 'ubi0:rootfs_data /overlay ubifs rw,relatime 0 0\n' >> "$SB/proc/mounts" ;;
        *)   printf '/dev/mtdblock4 /overlay jffs2 rw,relatime 0 0\n' >> "$SB/proc/mounts" ;;
    esac
}
set_mounts

# MemAvailable is the budget check_unpack_ram measures an unpack against. The
# default is generous, so every test that is not about memory sees the same
# camera it always did; the memory tests set it to the figure the reporter's
# gk7205v200 had. Deliberately more than one line, because the awk that reads it
# has to pick MemAvailable out and not MemFree above it.
set_meminfo() {
    printf 'MemTotal:       %8d kB\nMemFree:        %8d kB\nMemAvailable:   %8d kB\n' \
        131072 "${1:-65536}" "${1:-65536}" > "$SB/proc/meminfo"
}
set_meminfo

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
# Three shapes reach this.
#
#  -r    gzip_isize_kb asking for the trailer. STUB_ISIZE set means a server
#        that implements Range: four little-endian bytes into the -o target and
#        a 206. Unset means one that does not -- GitHub's asset host answers
#        501 -- so the caller must fall back.
#  -sIL  remote_length_kb's HEAD. Always answered as a redirect that carries a
#        body length of its own (legal, and what makes "the last length in the
#        stream" the wrong reading) followed by the artifact. STUB_DL_BYTES
#        unset makes that final response chunked, i.e. a server that will not
#        say, which check_unpack_ram has to treat as "no opinion".
#  else  a body fetch whose only interesting property is its exit status.
stub curl '
out=""; prev=""; ranged=0; head=0
for a in "$@"; do
    [ "$prev" = "-o" ] && out=$a
    [ "$a" = "-r" ] && ranged=1
    [ "$a" = "-sIL" ] && head=1
    prev=$a
done
if [ "$ranged" = "1" ]; then
    if [ -n "${STUB_RANGE_IGNORED:-}" ] && [ -n "$out" ]; then
        # A server with no Range support: 200, and the whole file. The real
        # request caps this with --max-filesize, so only the head of it lands --
        # which begins with the gzip magic, 0x08088b1f.
        printf %b "\\x1f\\x8b\\x08\\x08\\x00\\x00\\x00\\x00" > "$out"
        printf 200
        exit 0
    fi
    if [ -n "${STUB_ISIZE:-}" ] && [ -n "$out" ]; then
        n=$STUB_ISIZE
        printf %b "$(printf "\\x%02x\\x%02x\\x%02x\\x%02x" \
            $((n & 255)) $(((n >> 8) & 255)) $(((n >> 16) & 255)) $(((n >> 24) & 255)))" > "$out"
        printf 206
    else
        printf 501
    fi
    exit 0
fi
if [ "$head" = "1" ]; then
    printf "HTTP/1.1 302 Found\r\ncontent-length: 65536\r\nlocation: /dl\r\n\r\n"
    if [ -n "${STUB_DL_BYTES:-}" ]; then
        printf "HTTP/1.1 200 OK\r\ncontent-length: %s\r\n\r\n" "$STUB_DL_BYTES"
    else
        printf "HTTP/1.1 200 OK\r\ntransfer-encoding: chunked\r\n\r\n"
    fi
    exit 0
fi
exit "${STUB_CURL_RC:-0}"'
# check_sdcard re-reads `mount` after every umount, so a static pair of stubs
# would spin forever: the unmount has to actually change what mount reports.
# $SDMOUNTS is what a bare `mount` prints; empty is the default, which is what
# every test that does not care about an SD card sees.
SDMOUNTS="$SB/tmp/sdmounts"
: > "$SDMOUNTS"
cat > "$SB/bin/umount" <<EOF
#!/bin/bash
if [ -n "\$1" ]; then
    grep -v " \$1 " "\$SDMOUNTS" > "\$SDMOUNTS.n" 2>/dev/null
    mv "\$SDMOUNTS.n" "\$SDMOUNTS" 2>/dev/null
fi
exit 0
EOF
chmod +x "$SB/bin/umount"
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
#
# STUB_FLASHCP_FAIL_DEV fails only the write to one device, which is what a
# split-layout run needs: the kernel goes down first, so failing everything
# cannot tell "the rootfs write failed" from "we never got that far".
stub busybox '
applet=$1; shift
case "$applet" in
    flashcp|flash_eraseall) echo "$applet $*" >> "$FLASH_LOG"
                            # Real flashcp writes progress to stdout, and under
                            # -s that is what set_progress pipes through awk for
                            # the WebUI. Without some, "does the progress still
                            # come out?" would be asserted against an empty pipe.
                            printf "Erasing block 1/1 (100%%)\nWriting kb 8/8 (100%%)\n"
                            [ "1" = "$STUB_FLASHCP_FAIL" ] && exit 1
                            if [ -n "$STUB_FLASHCP_FAIL_DEV" ]; then
                                case " $* " in *" $STUB_FLASHCP_FAIL_DEV "*) exit 1 ;; esac
                            fi ;;
    reboot)                 echo "reboot" >> "$FLASH_LOG"; exit 0 ;;
esac
exit 0'

# The verify-mount. STUB_MOUNT picks the behaviour under test.
#   ok   — mount succeeds; populate the mountpoint like a real rootfs
#   fail — mount fails the way a missing squashfs decompressor does
#   hang — mount blocks; only a bounded caller survives this
# A bare `mount` (check_sdcard's `mount | grep /mnt/mmc`) must stay quiet.
stub mount '
[ $# -eq 0 ] && { cat "$SDMOUNTS" 2>/dev/null; exit 0; }
target=${!#}
# STUB_MOUNT models the VERIFY-mount (a squashfs image over a loop device). The
# ramfs pivot mounts tmpfs and relocates existing mounts; those are a different
# operation and must not inherit the fault being injected, or the hang case
# wedges before the code under test is even reached.
for a in "$@"; do
    case "$a" in
        # quiesce_overlay remounts the jffs2 read-only before the erase. Log it
        # so a test can assert both that it happened and that it happened FIRST;
        # STUB_REMOUNT_RC injects a kernel that refuses.
        *remount*) echo "remount $*" >> "$FLASH_LOG"; exit "${STUB_REMOUNT_RC:-0}" ;;
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

# A squashfs whose superblock claims $2 bytes, padded to $3 bytes on disk ($3
# defaults to $2). $3 < $2 is what an unpack that runs out of room in /tmp
# leaves behind, and what check_rootfs_complete has to refuse; $3 > $2 is what
# repack normally produces, because it pads the artifact to a 4K boundary. Only
# the two fields sysupgrade reads are real: the magic at 0, bytes_used at 0x28.
make_squashfs() {
    local claimed=$2 ondisk=${3:-$2}
    printf '\x68\x73\x71\x73' > "$1"                      # 'hsqs'
    dd if=/dev/zero bs=1 count=36 >> "$1" 2>/dev/null       # 4..39
    printf %b "$(printf '\\x%02x\\x%02x\\x%02x\\x%02x' \
        $((claimed & 255)) $(((claimed >> 8) & 255)) \
        $(((claimed >> 16) & 255)) $(((claimed >> 24) & 255)))" >> "$1"
    dd if=/dev/zero bs=1 count=4 >> "$1" 2>/dev/null        # 44..47, the high half
    dd if=/dev/zero bs=1 count=$((ondisk - 48)) >> "$1" 2>/dev/null
}

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
        SDMOUNTS="$SDMOUNTS" \
        abort_wait=0 RAM_ROOT="$SB/ram" \
        WDOG="${WDOG:-$SB/dev/watchdog}" WDOG_PROC="${WDOG_PROC:-$SB/proc}" \
        STUB_PIVOT_RC="${STUB_PIVOT_RC:-1}" \
        STUB_MOUNT="${STUB_MOUNT:-ok}" STUB_VENDOR="${STUB_VENDOR:-sigmastar}" \
        STUB_SOC="${STUB_SOC:-ssc338q}" \
        STUB_IMG_SOC="${STUB_IMG_SOC:-ssc338q}" \
        STUB_IMG_VERSION="${STUB_IMG_VERSION:-2026.07.11}" \
        STUB_FLASHCP_FAIL="${STUB_FLASHCP_FAIL:-0}" \
        STUB_FLASHCP_FAIL_DEV="${STUB_FLASHCP_FAIL_DEV:-}" \
        STUB_REMOUNT_RC="${STUB_REMOUNT_RC:-0}" \
        STUB_CURL_RC="${STUB_CURL_RC:-0}" \
        STUB_DL_BYTES="${STUB_DL_BYTES:-}" \
        STUB_ISIZE="${STUB_ISIZE:-}" \
        STUB_RANGE_IGNORED="${STUB_RANGE_IGNORED:-}" \
        UNPACK_RESERVE_KB="${UNPACK_RESERVE_KB:-512}" \
        sh "$SB/sysupgrade" "$@" 2>&1)
    RC=$?
}
flashed()      { grep -q "flashcp .*$1" "$SB/tmp/flash.log"; }
erased()       { grep -q "flash_eraseall .*$1" "$SB/tmp/flash.log"; }
remounted_ro() { grep -q "remount .*remount,ro .*$1" "$SB/tmp/flash.log"; }
# Line number of a phrase in the flash log, for ordering assertions: quiesce
# before erase is the whole point, and a remount that lands afterwards is a
# remount of a partition that has already been deleted.
logged_at()    { grep -n -- "$1" "$SB/tmp/flash.log" | head -1 | cut -d: -f1; }
nothing_wrote() { ! grep -q "flashcp" "$SB/tmp/flash.log"; }
# The busybox stub logs a bare "reboot" line, so whether the run rebooted is
# directly observable -- which is the whole question issue #2231 turns on.
rebooted()     { grep -qx "reboot" "$SB/tmp/flash.log"; }
# Line number of a phrase in $OUT, for ordering assertions.
at() { printf '%s\n' "$OUT" | grep -n -- "$1" | head -1 | cut -d: -f1; }

reset_env() {
    unset STUB_MOUNT STUB_VENDOR STUB_SOC STUB_IMG_SOC STUB_IMG_VERSION MOUNT_WAIT
    unset STUB_FLASHCP_FAIL STUB_FLASHCP_FAIL_DEV STUB_PIVOT_RC STUB_REMOUNT_RC STUB_CURL_RC
    unset STUB_DL_BYTES STUB_ISIZE STUB_RANGE_IGNORED UNPACK_RESERVE_KB
    set_meminfo
    : > "$SDMOUNTS"
    set_mounts
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

# --- the staged archive is not kept ----------------------------------------
# The WebUI's "install from a file" route uploads the .tgz into /tmp and then
# points --archive at it, so from the unpack onwards /tmp holds the image twice
# over -- on a 64 MB SigmaStar that is most of the tmpfs, and the flash phase
# still wants room for the dd split and the verify mount
# (OpenIPC/majestic-webui#474). $SB/tmp IS /tmp here: the harness rewrites the
# literal in the script under test, so the guard fires exactly as it would on a
# camera.
reset_env
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ ! -f "$SB/tmp/fw.tgz" ]; then
    ok "an archive staged in /tmp is freed once it has been unpacked"
else
    bad "staged archive should not survive the unpack, rc=$RC present=$([ -f "$SB/tmp/fw.tgz" ] && echo yes || echo no)"
fi

# ...but only that one. --archive can name a file on an SD card or a share, and
# deleting the operator's own copy of an image is not this script's business.
reset_env
mkdir -p "$SB/keep"
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
mv "$SB/tmp/fw.tgz" "$SB/keep/fw.tgz"
run -z --archive="$SB/keep/fw.tgz"
if [ "$RC" -eq 0 ] && [ -f "$SB/keep/fw.tgz" ]; then
    ok "an archive the caller owns is left where they put it"
else
    bad "archive outside /tmp must be kept, rc=$RC present=$([ -f "$SB/keep/fw.tgz" ] && echo yes || echo no)"
fi

# ...and "under /tmp" is about where the file IS, not how it was spelt. A path
# that walks back out lands on the caller's own media, which is the one thing
# this guard exists not to delete.
reset_env
mkdir -p "$SB/keep"
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
mv "$SB/tmp/fw.tgz" "$SB/keep/fw.tgz"
run -z --archive="$SB/tmp/../keep/fw.tgz"
if [ "$RC" -eq 0 ] && [ -f "$SB/keep/fw.tgz" ]; then
    ok "an archive reached through /tmp/.. is still the caller's"
else
    bad "a /tmp/.. alias must not delete an outside archive, rc=$RC present=$([ -f "$SB/keep/fw.tgz" ] && echo yes || echo no)"
fi

# --- an image shorter than its own superblock -------------------------------
#
# Reported 2026-09-18: a gk7205v200 froze at "Erasing block: 62/64 (96%)" and
# came back with a corrupted filesystem. 64 blocks is 4 MB of erase, and the
# rootfs published for that camera is 4.70 MB -- it flashed an image ~500 KB
# short. Every gate it passed on the way is load-bearing here: an unpack that
# fills /tmp drops the image's .md5sum (packed after the image, so it is the
# member that does not land), which narrows `md5sum -c *.md5sum` to the kernel
# and still exits 0; the loop-mount then fails, which is deliberately NOT fatal
# because a running kernel may lack the new decompressor; and flashcp verifies
# the file against the flash rather than against a filesystem, so it reports
# success. The squashfs's own recorded length is the one witness that survives
# all three.
reset_env
make_squashfs "$R" 8192 4096
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "Incomplete rootfs"; then
    ok "a rootfs shorter than its own superblock is refused before any write"
else
    bad "short rootfs -> expected a clean refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
grep -q "S95majestic restart" "$SB/tmp/flash.log" \
    && ok "...and the refusal hands the stopped services back" \
    || bad "a short-rootfs refusal must restore the services it stopped"

# Padding is normal, so the test is "shorter than", never "not equal to".
reset_env
make_squashfs "$R" 8192 12288
run -z --kernel="$K" --rootfs="$R"
{ [ "$RC" -eq 0 ] && flashed /dev/mtd3; } \
    && ok "a squashfs padded past its recorded length still flashes" \
    || bad "padded rootfs -> expected a flash, rc=$RC log='$(cat "$SB/tmp/flash.log")'"

# And no magic means no opinion: the guard must not start rejecting artifacts it
# cannot actually measure.
reset_env
run -z --kernel="$K" --rootfs="$R"
{ [ "$RC" -eq 0 ] && flashed /dev/mtd3; } \
    && ok "a rootfs with no squashfs magic is left alone" \
    || bad "an unmeasurable rootfs must keep today's behaviour, rc=$RC"

# --- a checksum that never arrived is not a checksum that passed ------------
#
# `md5sum -c *.md5sum` verifies what the manifests list and says nothing about a
# file no manifest names, so the gate does not fail when a companion goes
# missing -- it silently narrows. This is the archive that proves it.
reset_env
rm -rf "$SB/stage2"; mkdir -p "$SB/stage2"
cp "$K" "$SB/stage2/uImage.ssc338q"
cp "$R" "$SB/stage2/rootfs.squashfs.ssc338q"
(cd "$SB/stage2" && md5sum uImage.ssc338q > openipc.md5sum)
(cd "$SB/stage2" && tar cf - . | gzip > "$SB/tmp/fw.tgz")
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "Nothing checksums rootfs.squashfs.ssc338q"; then
    ok "an image no manifest covers is refused, not waved through"
else
    bad "uncovered rootfs -> expected a refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# A manifest may legitimately name its members with a leading "./" -- that is
# what `md5sum` writes when it is run from a staging directory, and `md5sum -c`
# verifies it fine from /tmp. Refusing those would block a good archive from
# installing, which is worse than the hole the coverage check closes.
reset_env
rm -rf "$SB/stage3"; mkdir -p "$SB/stage3"
cp "$K" "$SB/stage3/uImage.ssc338q"
cp "$R" "$SB/stage3/rootfs.squashfs.ssc338q"
(cd "$SB/stage3" && md5sum ./uImage.ssc338q ./rootfs.squashfs.ssc338q > openipc.md5sum)
(cd "$SB/stage3" && tar cf - . | gzip > "$SB/tmp/fw.tgz")
run -z --archive="$SB/tmp/fw.tgz"
{ [ "$RC" -eq 0 ] && flashed /dev/mtd3; } \
    && ok "a manifest that names its members ./x still counts as coverage" \
    || bad "./-prefixed manifest names must not be refused, rc=$RC log='$(cat "$SB/tmp/flash.log")'"

# --- an unpack with nowhere to go ------------------------------------------
#
# The archive routes hold the .tgz and everything inside it on the same tmpfs at
# once: 6395 KB + 6413 KB against a 13564 KB /tmp on the camera this was
# reported from. Past that edge tar dies mid-member and leaves the truncated
# image the cases above have to catch, so refuse while the numbers are still
# knowable. #2425 frees the archive after the unpack, which cannot help the
# unpack itself.
reset_env
make_archive "$K" "$R"
stub df 'echo "Filesystem 1K-blocks Used Available Use% Mounted on"; echo "tmpfs 13564 13560 4 99% /tmp"'
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "No room to unpack"; then
    ok "an unpack that cannot fit is refused before it truncates an image"
else
    bad "full /tmp -> expected a refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
rm -f "$SB/bin/df"

# Fails open, like every other measurement in this script: a df that will not
# answer is not grounds to refuse an upgrade.
reset_env
make_archive "$K" "$R"
stub df 'exit 1'
run -z --archive="$SB/tmp/fw.tgz"
[ "$RC" -eq 0 ] \
    && ok "...and a df that will not answer is not a refusal" \
    || bad "an unreadable df must not block an upgrade, rc=$RC"
rm -f "$SB/bin/df"

# --- an unpack with nowhere to go, the other kind (issue #2457) -------------
#
# /tmp is a tmpfs, so the room question has a second half: the RAM the tmpfs is
# made of. On a `mem=32M` camera that is the half that binds, and df cannot see
# it -- the camera in #2457 was 400 KB short of the memory it needed while
# reporting 51836 KB free in /tmp.
#
# It has to be refused BEFORE the unpack, because afterwards there is nobody
# left to refuse it: tmpfs pages belong to no process, so the OOM killer takes
# the largest RSS on the box instead, which is majestic -- and on a --web run
# majestic is what is streaming the log. The observed failure is a transcript
# that stops mid-sentence, a camera still on the old image, and RTSP and the
# WebUI gone until it is power-cycled.
reset_env
set_meminfo 8192          # what free -h reported on the reporter's gk7205v200
STUB_DL_BYTES=8691055     # openipc.gk7205v200-nor-ultimate.tgz
run -z --web -k -r
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "a streamed image larger than free RAM is refused before it is unpacked"
else
    bad "8487 KB into 8192 KB -> expected a refusal, rc=$RC out='$OUT'"
fi

# The advice has to be reachable. A WebUI run is the only one with majestic
# still resident, so it is the only one told to go and use a shell; saying that
# to somebody already in one would be noise.
printf '%s' "$OUT" | grep -q "from ssh or the serial console" \
    && ok "...and says where the missing memory is" \
    || bad "a --web refusal should point at the shell path, out='$OUT'"

reset_env
set_meminfo 8192
STUB_DL_BYTES=8691055
run -z -k -r
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "Not enough memory to unpack" &&
    ! printf '%s' "$OUT" | grep -q "from ssh or the serial console"; then
    ok "...but a console run is not told to go and find a console"
else
    bad "a non-web refusal should not carry the --web advice, rc=$RC out='$OUT'"
fi

# The same measurement, on the archive route -- the WebUI's "install from a
# file", which hands majestic's upload straight to --archive and so has majestic
# resident too. Here df has an opinion and it is the wrong one: the sandbox's
# /tmp is a real filesystem with gigabytes free, exactly as the camera's tmpfs
# claimed 51836 KB while the machine had 9 MB to give.
#
# A zero-filled rootfs, because what check_unpack_room reads is the gzip
# trailer: 12 MB of zeros costs the suite a few KB on disk and still asks the
# question at the scale a real image asks it.
reset_env
dd if=/dev/zero bs=1k count=12288 of="$R" 2>/dev/null
make_archive "$K" "$R"
set_meminfo 8192
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "a staged archive too big for RAM is refused even where df says there is room"
else
    bad "archive route ignored the memory budget, rc=$RC out='$OUT'"
fi

# Fails open on both halves of the arithmetic, like every other measurement in
# this script. A server that will not give a size, and a /proc/meminfo that will
# not parse, are each "no opinion" -- never a refusal.
reset_env
set_meminfo 8192
run -z --web -k -r       # STUB_DL_BYTES unset: no Content-Length comes back
if ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "a server that will not give a size is not a refusal"
else
    bad "an unknown download size must not block an upgrade, out='$OUT'"
fi

reset_env
: > "$SB/proc/meminfo"
STUB_DL_BYTES=8691055
run -z --web -k -r
if ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "...and neither is a /proc/meminfo that will not parse"
else
    bad "an unreadable meminfo must not block an upgrade, out='$OUT'"
fi
set_meminfo

# The budget is not just the image. curl, gzip and tar are forked after
# MemAvailable is read and live alongside the pages they write -- 96 pages
# between them in the #2457 OOM dump -- so an unpack that fits with nothing to
# spare does not fit. 8691055 B is 8487 KB, and the reserve is 512.
reset_env
set_meminfo 9000
STUB_DL_BYTES=8691055
run -z --web -k -r
if ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "an image that fits with the reserve to spare is not refused"
else
    bad "8487+512 KB into 9000 KB should pass, out='$OUT'"
fi

reset_env
set_meminfo 8999
STUB_DL_BYTES=8691055
run -z --web -k -r
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "...and one kilobyte tighter is refused, with the reserve counted in"
else
    bad "8487+512 KB into 8999 KB should refuse, rc=$RC out='$OUT'"
fi

# Content-Length is a floor, not a bound. It is the unpacked size to within a
# fraction of a percent for OpenIPC's own tarballs -- already-compressed
# payloads -- but --url takes any archive, and a compressible one expands far
# past it. Ask the gzip trailer first, wherever the server will serve a Range.
reset_env
set_meminfo 8192
STUB_DL_BYTES=1048576     # 1 MB on the wire...
STUB_ISIZE=52428800       # ...50 MB once unpacked
run -z --web -k -r
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "51200 KB of image"; then
    ok "the gzip trailer outranks Content-Length when the server serves a Range"
else
    bad "a compressible custom archive must be sized by its trailer, rc=$RC out='$OUT'"
fi

# And the trailer is only believed when it really is the trailer. A server that
# ignores Range answers 200 with the whole file, where the first four bytes are
# the gzip magic -- 0x08088b1f, which would read as a 135 MB unpack.
reset_env
set_meminfo 8192
STUB_DL_BYTES=1048576
run -z --web -k -r          # STUB_ISIZE unset: the stub answers 501
if ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "...and a server with no Range support falls back instead of guessing"
else
    bad "a 501 to the range probe must fall back to Content-Length, out='$OUT'"
fi

# The other way a Range probe goes wrong: a 200 with the file itself, whose
# first four bytes are the gzip magic 0x08088b1f -- 135 MB, if believed.
reset_env
set_meminfo 8192
STUB_DL_BYTES=1048576
STUB_RANGE_IGNORED=1
run -z --web -k -r
if ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "...and a 200 to a ranged request is not read as a trailer"
else
    bad "the gzip magic must not be mistaken for a length, out='$OUT'"
fi

# That probe writes into the tmpfs this guard protects, so it has to be capped
# at the four bytes it wants. Without --max-filesize a ranged GET that the
# server answers with the whole artifact downloads the whole artifact: measured
# on a lab gk7205v300, a 200 MB one filled its 60 MB /tmp and reset the board.
grep -q -- '--max-filesize' "$SRC" \
    && ok "the range probe cannot download more than the four bytes it wants" \
    || bad "gzip_isize_kb must cap its response size"

# Which response the length came from matters. `curl -IL` prints every hop, so
# the last length in the stream is the redirect's whenever the artifact itself
# is chunked -- and the redirect below declares one, as a redirect with a body
# may. Reading that would size a 8.5 MB image at 64 KB, or refuse on it.
reset_env
set_meminfo 64              # so any estimate at all would refuse
run -z --web -k -r          # STUB_DL_BYTES unset: the final response is chunked
if ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "a chunked artifact behind a redirect is not measured as the redirect"
else
    bad "Content-Length must come from the final response only, out='$OUT'"
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
# ...and because it is live, the kernel has to be taken off it first. jffs2's GC
# thread writes the same medium flash_eraseall is erasing, and both notice: the
# thread walks a filesystem being deleted under it while jffs2's own erase path
# reads back blocks holding flash_eraseall's cleanmarkers. remount,ro stops that
# thread (jffs2_remount_fs does it on the way to ro); umount cannot, because the
# jffs2 is the upperdir of the overlayfs that is still root for init(1).
reset_env
run -z --wipe_overlay
if remounted_ro /overlay && erased /dev/mtd4 \
    && [ "$(logged_at remount)" -lt "$(logged_at flash_eraseall)" ]; then
    ok "--wipe_overlay quiesces the overlay read-only BEFORE erasing it"
else
    bad "--wipe_overlay must remount the overlay ro before the erase, log='$(cat "$SB/tmp/flash.log")'"
fi

# A kernel that refuses the remount must not cost the user their reset: erasing
# it live is what this did before the quiesce existed, so that is the fallback.
reset_env
STUB_REMOUNT_RC=1
run -z --wipe_overlay
if erased /dev/mtd4 && printf '%s' "$OUT" | grep -q "erasing it live"; then
    ok "a refused remount warns and still erases (no worse than the old behaviour)"
else
    bad "a refused remount must not block the erase, rc=$RC out='$OUT'"
fi

# A NAND camera mounts the very same partition as "ubi0:rootfs_data", not as an
# mtdblock device (general/overlay/init). A lookup that only knows the block
# device spelling passes every test above and still no-ops on every UBI board.
reset_env
set_mounts ubi
run -z --wipe_overlay
if remounted_ro /overlay && erased /dev/mtd4 \
    && [ "$(logged_at remount)" -lt "$(logged_at flash_eraseall)" ]; then
    ok "a ubifs overlay is quiesced too (init mounts it as ubi0:rootfs_data)"
else
    bad "--wipe_overlay must quiesce a ubifs overlay as well, log='$(cat "$SB/tmp/flash.log")'"
fi

# Nothing to quiesce when the partition carries no mounted filesystem at all --
# a root that never brought an overlay up, or one whose upper layer is tmpfs.
# The lookup must come up empty and step aside rather than remount something else.
reset_env
set_mounts no-overlay
run -z --wipe_overlay
if erased /dev/mtd4 && ! grep -q remount "$SB/tmp/flash.log"; then
    ok "an unmounted rootfs_data is erased with no remount attempted"
else
    bad "nothing is mounted on rootfs_data; no remount should have been tried, log='$(cat "$SB/tmp/flash.log")'"
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

# --- the pivot is entered only when it is needed (issue #2416) --------------
#
# There is no way back out of the ramfs. The shell, /etc, dropbear and getty are
# all behind /mnt, so a run that pivots and then hands the camera back leaves a
# box that answers ping and nothing else: the console loops on `can't run
# '/sbin/getty'` and ssh rejects a key that worked a minute earlier, until
# somebody power-cycles it.
#
# A kernel-only write never touches the mounted partition, so it never needed
# the pivot the live rootfs write does -- and it is precisely the run where -x
# is honoured, which is how people were stranded.
reset_env
run -z --kernel="$K" -x
if [ "$RC" -eq 0 ] && ! grep -q "pivot_root" "$SB/tmp/flash.log" \
    && flashed /dev/mtd2 && ! rebooted; then
    ok "-x + kernel only -> no pivot, so the camera keeps its shell (#2416)"
else
    bad "a kernel-only run must not pivot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
printf '%s' "$OUT" | grep -q "Flashing in place" \
    && ok "...and says why there is no ramfs this time" \
    || bad "a run that skips the pivot should say so; got: $(printf '%s' "$OUT" | tail -3)"

# The other half: a write that DOES land on the live rootfs still moves into RAM.
reset_env
run -z --rootfs="$R"
grep -q "pivot_root" "$SB/tmp/flash.log" \
    && ok "a live-rootfs write still moves the flash phase into RAM" \
    || bad "the rootfs write must still pivot, log='$(cat "$SB/tmp/flash.log")'"

# Not every camera runs from the flash it writes; then the partition is just a
# target and the pivot buys nothing.
reset_env
set_cmdline "$CMDLINE_NFS"
run -z --rootfs="$R"
if ! grep -q "pivot_root" "$SB/tmp/flash.log" && flashed /dev/mtd3 && [ "$RC" -eq 0 ]; then
    ok "an NFS-rooted camera writes the rootfs partition without a pivot"
else
    bad "NFS root needs no pivot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# -n erases the jffs2 that is the overlay's UPPER layer, which overlayfs
# consults on every lookup. That is as live as the squashfs, so it pivots.
reset_env
run -z -n
grep -q "pivot_root" "$SB/tmp/flash.log" \
    && ok "--wipe_overlay pivots too (the overlay it erases is live)" \
    || bad "-n must pivot; the jffs2 it erases backs the running root"

# The trap in "a kernel write is not a live write": a board with no separate
# kernel partition has kernel_device pointing at the combined "firmware" one
# (get_system_info's fallback), which overlaps the rootfs the camera runs from.
# There a plain --kernel is a live-flash write -- it must pivot, and -x must not
# be honoured after it -- and the run that looks most harmless is the one that
# would have rewritten the running filesystem in place.
reset_env
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00700000 00010000 "firmware"
EOF
run -z --kernel="$K" -x
if grep -q "pivot_root" "$SB/tmp/flash.log" && flashed /dev/mtd2 && rebooted; then
    ok "kernel-only on a combined layout is a live write: pivots, and -x is overridden"
else
    bad "kernel into the firmware partition must pivot and reboot, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# --- certificate verification (GHSA-fjf7-9x3v-6mj6) ------------------------
# Every online fetch used to pass -k, so the network could hand the camera any
# image -- and, through self_update, any script to exec as root. The probe
# before the download is where a verification failure now surfaces. It must
# abort before anything is written, and it must say "certificate" rather than
# "Check your network!", because the two have different fixes. The sandbox's
# GNU date has no -D, so the HTTP-Date fallback declines here and the failure
# is final -- which is the path an NTP-blocked camera with no HTTP either sees.
reset_env
STUB_CURL_RC=60          # curl: the peer certificate cannot be authenticated
run -z -r
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s\n' "$OUT" | grep -q 'Certificate verification failed'; then
    ok "a certificate failure aborts before the download and names the cause"
else
    bad "certificate failure -> expected an abort naming the certificate, rc=$RC out='$OUT'"
fi
printf '%s\n' "$OUT" | grep -q -- '--insecure' \
    && ok "...and points at --insecure for a private mirror" \
    || bad "the certificate message must mention --insecure"

reset_env
STUB_CURL_RC=77          # curl: the CA bundle cannot be read
run -z -r
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s\n' "$OUT" | grep -q 'CA bundle'; then
    ok "an unreadable CA bundle is reported as such"
else
    bad "unreadable CA bundle -> expected an abort naming the bundle, rc=$RC out='$OUT'"
fi

reset_env
STUB_CURL_RC=7           # curl: failed to connect
run -z -r
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s\n' "$OUT" | grep -q 'Check your network'; then
    ok "an unreachable server is still a network problem, not a certificate one"
else
    bad "connect failure -> expected 'Check your network', rc=$RC out='$OUT'"
fi

reset_env
run -z --insecure -r
printf '%s\n' "$OUT" | grep -q 'NOT be verified' \
    && ok "--insecure announces itself" \
    || bad "--insecure must warn that verification is off, out='$OUT'"

# self_update may only ever consider the copy THIS run fetched. A script left in
# /tmp by an earlier run (one made with --insecure, say) used to be version-
# compared and exec'd as root whenever the current fetch failed (Qodo, #2374).
reset_env
STUB_CURL_RC=7           # this run's fetch fails
printf '#!/bin/sh\nscr_version=0.0.0\necho STALE SCRIPT RAN\nexit 42\n' > "$SB/tmp/sysupgrade"
run -r                   # no -z: self_update runs
if [ "$RC" -ne 42 ] && nothing_wrote && ! printf '%s\n' "$OUT" | grep -q 'STALE SCRIPT RAN' \
   && printf '%s\n' "$OUT" | grep -q 'Version checking failed'; then
    ok "a failed self-update fetch never falls through to a script left by an earlier run"
else
    bad "stale /tmp/sysupgrade was consulted after a failed fetch, rc=$RC out='$OUT'"
fi
[ -e "$SB/tmp/sysupgrade" ] \
    && bad "the stale script survived the failed fetch and will be seen by the next run" \
    || ok "...and the leftover is gone"
rm -f "$SB/tmp/sysupgrade"

# --- aborting on a recovery file left on the SD card ------------------------
# check_sdcard runs AFTER create_lock and free_resources, so how it leaves is
# not a detail: a bare `exit` there stranded the lock in /tmp (every later run
# then refused with "Another sysupgrade process is already running!" until a
# reboot), left syslogd/klogd/ntpd/crond stopped, and left majestic gutted by
# free_resources' SIGQUIT -- a camera with no video and no logging, having been
# told only to take the card out.
#
# The mount line only has to CONTAIN /mnt/mmc for check_sdcard's grep; the
# directory it hands on is field 3, so it can point inside the sandbox and the
# recovery file can actually exist.
reset_env
SD="$SB/mnt/mmcblk0p1"
mkdir -p "$SD"
: > "$SD/autoupdate-rootfs.img"
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote; then
    ok "a recovery file on the card aborts before anything is written"
else
    bad "recovery file -> expected a clean abort, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
if [ ! -f "$SB/tmp/sysupgrade.lock" ]; then
    ok "...and the abort takes its lock file with it"
else
    bad "the SD-card abort left the lock file behind; the next run is locked out"
fi
if grep -q "S95majestic restart" "$SB/tmp/flash.log"; then
    ok "...and restarts the services free_resources stopped"
else
    bad "the SD-card abort skipped restore_resources; services stay stopped, majestic stays gutted"
fi
rm -f "$SD/autoupdate-rootfs.img"

# The other half: a card with nothing incriminating on it is unmounted and the
# run carries on. This is also what proves the loop terminates -- check_sdcard
# re-reads `mount` after each umount, so a card that never goes away spins.
reset_env
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3; then
    ok "a clean card is unmounted and the upgrade proceeds"
else
    bad "clean card -> expected the run to proceed, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# ---------------------------------------------------------------------------
echo
echo "=== Part 1f: a refused write is not a completed one (issue #2426) ==="

# Reported 2026-09-16: an image larger than the partition it is bound for makes
# flashcp refuse -- it checks the size before it erases anything -- and
# sysupgrade reported the upgrade as done. On the split path nothing ever looked
# at flashcp's status, and the success lines are worse than a bare "OK": the
# kernel one reads the version back off the DEVICE, so it prints the timestamp
# of the kernel still sitting there, and the rootfs one prints the version
# verify_rootfs read out of the CANDIDATE FILE -- exactly the version the
# operator was hoping to see.

# A kernel write that fails must be fatal, and must not claim a version.
reset_env
STUB_FLASHCP_FAIL_DEV=/dev/mtd2
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && ! printf '%s' "$OUT" | grep -q "Kernel updated to"; then
    ok "a failed kernel write fails the run instead of announcing a version"
else
    bad "failed kernel write -> expected a non-zero exit and no success line, rc=$RC out='$(printf '%s' "$OUT" | tail -3)'"
fi
# ...and it must not go on to write the rootfs on top of it.
! flashed /dev/mtd3 \
    && ok "...and stops there rather than carrying on to the rootfs" \
    || bad "the run continued to the rootfs after the kernel write failed"

# The same for the rootfs, with the kernel write left working so the failure is
# unambiguously the rootfs one.
reset_env
STUB_FLASHCP_FAIL_DEV=/dev/mtd3
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && ! printf '%s' "$OUT" | grep -q "RootFS updated to"; then
    ok "a failed rootfs write fails the run instead of announcing a version"
else
    bad "failed rootfs write -> expected a non-zero exit and no success line, rc=$RC out='$(printf '%s' "$OUT" | tail -3)'"
fi
# It erased before it failed, so it has to reboot -- the #2231 rule, reached
# from the split path for the first time.
rebooted \
    && ok "...and reboots, because a half-erased live partition is not survivable" \
    || bad "a failed live-rootfs write must still reboot, log='$(cat "$SB/tmp/flash.log")'"

# -s is the mode the WebUI drives, and it is the one where the guard was dead
# code: set_progress pipes busybox through awk, and a pipeline reports its LAST
# command's status, so every `|| die` behind it saw awk's 0.
reset_env
STUB_FLASHCP_FAIL_DEV=/dev/mtd3
run -z -s --rootfs="$R"
if [ "$RC" -ne 0 ]; then
    ok "the write's status survives the progress pipe in silent mode"
else
    bad "-s masked a failed write behind awk's exit status, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
# And the mode still does what it is for: the write's own output, numbered.
printf '%s' "$OUT" | grep -q '^1 Erasing' \
    && ok "...and still prints the numbered progress the WebUI polls" \
    || bad "-s no longer emits progress lines, out='$(printf '%s' "$OUT" | tail -3)'"

# The reported case, end to end: an image too big for its partition. flashcp
# would refuse it without erasing anything, so the right answer is to refuse it
# BEFORE the pivot -- once inside the ramfs a die() has to reboot, and a reboot
# with nothing written is indistinguishable from a successful upgrade to
# anything watching the camera come back.
reset_env
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00200000 00010000 "kernel"
mtd3: 00001000 00010000 "rootfs"
mtd4: 00100000 00010000 "rootfs_data"
EOF
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote && ! rebooted; then
    ok "a rootfs too big for its partition is refused with nothing written"
else
    bad "oversized rootfs -> expected a clean refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
# The message has to name the two numbers; "flashcp failed" sends the operator
# looking at the image when the answer is in their partition layout (#2238).
if printf '%s' "$OUT" | grep -q "does not fit its partition"; then
    ok "...and says which image, which partition, and by how much"
else
    bad "the refusal must name the sizes; got: $(printf '%s' "$OUT" | tail -3)"
fi
# Nothing was written, so the camera is untouched: it keeps its services.
grep -q "S95majestic restart" "$SB/tmp/flash.log" \
    && ok "...and hands the camera back with its services running" \
    || bad "the refusal left the camera degraded, log='$(cat "$SB/tmp/flash.log")'"

# The kernel is checked on the same terms.
reset_env
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00000010 00010000 "kernel"
mtd3: 00500000 00010000 "rootfs"
mtd4: 00100000 00010000 "rootfs_data"
EOF
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote; then
    ok "a kernel too big for its partition is refused with nothing written"
else
    bad "oversized kernel -> expected a clean refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# A combined image has to be measured before the pivot too, and both of its
# shapes are measurable there: whole-blob against the firmware partition...
reset_env
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00001000 00010000 "firmware"
EOF
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_wrote && ! rebooted \
    && printf '%s' "$OUT" | grep -q "does not fit its partition"; then
    ok "an oversized whole-blob combined image is refused before the pivot"
else
    bad "oversized combined blob -> expected a clean refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# ...and on a split layout, each slice against its own partition, measured at
# the same 64K-aligned FIT boundary do_update_firmware cuts on. The rootfs slice
# is the one that used to be found only after the kernel had been committed.
reset_env
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00200000 00010000 "kernel"
mtd3: 00001000 00010000 "rootfs"
mtd4: 00100000 00010000 "rootfs_data"
EOF
make_combined "$SB/tmp/firmware.bin.ssc338q"
make_archive "$SB/tmp/firmware.bin.ssc338q"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_wrote && ! rebooted; then
    ok "an oversized rootfs slice of a combined image is refused before the kernel is written"
else
    bad "oversized combined rootfs slice -> expected a clean refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# It must fail OPEN. The check is an early warning in front of flashcp's own
# refusal, not a new gate: a size it cannot read must never block a flash that
# would have worked. An image on a camera whose busybox has no stat applet is
# the case that matters.
reset_env
stub stat 'exit 1'
set_mtd <<'EOF'
dev:    size   erasesize  name
mtd0: 00040000 00010000 "boot"
mtd1: 00010000 00010000 "env"
mtd2: 00200000 00010000 "kernel"
mtd3: 00001000 00010000 "rootfs"
mtd4: 00100000 00010000 "rootfs_data"
EOF
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3; then
    ok "a size it cannot read does not block the flash"
else
    bad "the size check must fail open, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
rm -f "$SB/bin/stat"

# ---------------------------------------------------------------------------
echo
echo "=== Part 2: invariants in $SRC ==="

# An option named in a user-facing message must exist in the parser.
# --connect-timeout, --speed-limit/--speed-time and --max-filesize are curl's,
# not ours.
for opt in $(grep -oE '\-\-[a-z_]+' "$SRC" | sort -u); do
    case "$opt" in
        --force_*|--wipe_overlay|--no_reboot|--no_update|--no_ramfs|--help|--web|--url|--archive|--kernel|--rootfs|--channel|--build|--list*|--insecure|--connect*|--speed*|--proto*|--max*) continue ;;
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
# The breadcrumb pair (#2415, #2417). Its whole value rests on ORDER: the line
# has to leave before syslogd stops, or a camera that never comes back takes the
# explanation with it. A later reshuffle of free_resources() that moved the
# logger below the stop would still pass every other check here.
awk '/^free_resources\(\)/,/^}/' "$SRC" \
    | grep -E 'logger|S01syslogd stop' | head -1 | grep -q 'logger' \
    && ok "free_resources announces the flash before it stops syslogd" \
    || bad "the pre-flash breadcrumb must precede 'S01syslogd stop', or it is never sent"
awk '/^restore_resources\(\)/,/^}/' "$SRC" | grep -q 'logger' \
    && ok "restore_resources retracts the breadcrumb when the camera stays up" \
    || bad "an aborted run leaves the collector holding a death notice for a live camera"
awk '/^free_resources\(\)/,/^}/' "$SRC" | grep -q 'syslog_remote_set' \
    && ok "the pre-flash sleep is gated on forwarding actually being enabled" \
    || bad "every camera pays the datagram-drain second for a feature most have off"

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
# ...guarded, because the rootfs SHIPS /ram now. rmdir cannot delete a lower-layer
# directory, so overlayfs records a whiteout that hides the shipped mount point
# from every later boot -- which is what a reporter found in /overlay/root after
# both an upgrade and a factory reset (majestic-webui#120). ramfs_discard got this
# guard when /ram started shipping; this is the copy on the path that actually
# runs, since a successful pivot never reaches ramfs_discard.
grep -qF '[ "1" = "$ram_root_shipped" ] || rmdir "/mnt$RAM_ROOT"' "$SRC" \
    && ok "the ramfs phase only reclaims a mount point it created (no whiteout)" \
    || bad "post-pivot rmdir is unguarded; on a rootfs shipping /ram it writes a whiteout"
# The guard is only worth anything if the value survives the re-exec: the second
# phase cannot recompute it, because by then $RAM_ROOT names a directory in the
# NEW root rather than the one being judged.
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -qE '^	export .*\bram_root_shipped\b' \
    && ok "ram_root_shipped is exported into the ramfs phase" \
    || bad "ram_root_shipped is not exported; the post-pivot guard always reads empty"
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
# The success path needs the same rule. Handing a pivoted camera back to the
# operator because -x was asked for leaves the corpse of #2416: pings, answers
# nothing, and rejects a key that worked a minute earlier.
awk '/^reboot_system\(\)/,/^}/' "$SRC" | grep -q '_ramfs_phase' \
    && ok "reboot_system will not honour -x from inside the ramfs either (#2416)" \
    || bad "an -x honoured inside the pivot strands a camera nobody can log into"
# ...and the way that stops being the common case is not pivoting at all when
# the run does not rewrite the flash the camera is served from. Gate and warning
# have to be the same question, asked once, or they drift apart.
if grep -q '^if ! rewrites_live_flash; then' "$SRC" &&
    grep -q '\[ "1" = "\$skip_reboot" \] && rewrites_live_flash; then' "$SRC"; then
    ok "the pivot and the -x warning are gated on one shared question"
else
    bad "enter_ramfs and the -x notice must share rewrites_live_flash, or they drift"
fi
# The size check has to run while the camera is still whole. Inside the pivot a
# refusal can only reboot, and a reboot with nothing written is what a watcher
# reads as a successful upgrade (majestic-webui #120).
pf=$(grep -n '^preflight_image_sizes$' "$SRC" | head -1 | cut -d: -f1)
er=$(grep -n 'enter_ramfs; then' "$SRC" | tail -1 | cut -d: -f1)
if [ -n "$pf" ] && [ -n "$er" ] && [ "$pf" -lt "$er" ]; then
    ok "images are measured before the pivot, not after it"
else
    bad "preflight_image_sizes must run before enter_ramfs -- preflight@${pf:-none} pivot@${er:-none}"
fi
# Every flashcp the script runs has to have its status read. A bare call
# discards it and a pipeline hides it; either way a write that never happened is
# announced as one that did (#2426).
if grep -n 'set_progress flash' "$SRC" | grep -qv '||'; then
    bad "an unguarded set_progress write: $(grep -n 'set_progress flash' "$SRC" | grep -v '||')"
else
    ok "every flashcp/flash_eraseall write is followed by a status check"
fi
# ...which only means anything if set_progress carries the status out of its own
# pipe. `busybox "$@" | awk ...` returns awk's 0 however the write went, so in
# silent mode -- the mode the WebUI drives -- every one of those guards was
# dead code.
if awk '/^set_progress\(\)/,/^}/' "$SRC" | grep -qF 'return ${st:-1}'; then
    ok "set_progress returns the write's status, not awk's"
else
    bad "set_progress swallows the write's status in silent mode; every '|| die' behind it is dead code"
fi
# do_update_firmware and the pre-check must cut a combined image at the same
# place, or the pre-check measures a rootfs slice that is not the one written.
[ "$(grep -c 'fit_split_blocks' "$SRC")" -ge 3 ] \
    && ok "the combined-image split boundary has one definition" \
    || bad "the 64K FIT boundary is computed in more than one place; they will disagree"
# check_image_fits runs inside the pivot as the backstop for the combined-image
# split, and the staged root has only the applets enter_ramfs links by name.
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'stat sync tail' \
    && ok "the staged root carries stat, which the size check needs" \
    || bad "check_image_fits reads sizes with stat; without the applet it silently fails open in the ramfs"
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

# --- certificate verification invariants (GHSA-fjf7-9x3v-6mj6) -------------
# -k may exist only as the value --insecure assigns to $curl_insecure. A literal
# -k on a curl line turns verification off for every camera again.
grep -qE 'curl[^|#]* -k( |$)' "$SRC" \
    && bad "a curl call passes -k directly: certificate verification is off again" \
    || ok "no curl call disables certificate verification on its own"
grep -q '^\s*--insecure)' "$SRC" \
    && ok "--insecure is the explicit, per-run opt-out" \
    || bad "--insecure is gone; a private mirror with a self-signed certificate has no way in"
awk '/^self_update\(\)/,/^}/' "$SRC" | grep -q -- '--proto =https' \
    && ok "self_update fetches the script it will exec over https only" \
    || bad "self_update must pin --proto/--proto-redir to https: a redirect to http hands root to the network"
awk '/^self_update\(\)/,/^}/' "$SRC" | grep -qE 'mv [^ ]*sysupgrade\.part' \
    && ok "self_update stages the download and renames it only when complete" \
    || bad "self_update must not be able to exec a partially downloaded script"
awk '/^self_update\(\)/,/^}/' "$SRC" | grep -qE 'rm -f [^ ]*/sysupgrade( |$)' \
    && ok "self_update discards whatever an earlier run left before it fetches" \
    || bad "self_update must remove any stale /tmp/sysupgrade first, or a failed fetch falls through to it"
awk '/^probe_url\(\)/,/^}/' "$SRC" | grep -q 'clock_from_http' \
    && ok "a date failure retries once with the clock taken from HTTP, not with -k" \
    || bad "probe_url must fall back to clock_from_http, or an NTP-blocked camera can never upgrade"
awk '/^clock_from_http\(\)/,/^}/' "$SRC" | grep -q '"\$web" -gt "\$now"' \
    && ok "clock_from_http only ever moves the clock forward" \
    || bad "clock_from_http must be forward-only: a spoofed Date header must not revive an expired certificate"

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

# The quiesce is only a quiesce if it precedes the erase, and the mount point has
# to be looked up rather than assumed: after the pivot the overlay is at
# /mnt/overlay, on the in-place fallback it is still at /overlay. A hardcoded
# path would silently remount nothing on one of the two paths.
wo=$(sed -n '/^do_wipe_overlay()/,/^}/p' "$SRC")
if printf '%s\n' "$wo" | grep -q 'quiesce_overlay' \
    && [ "$(printf '%s\n' "$wo" | grep -n quiesce_overlay | head -1 | cut -d: -f1)" \
       -lt "$(printf '%s\n' "$wo" | grep -n flash_eraseall | head -1 | cut -d: -f1)" ]; then
    ok "do_wipe_overlay quiesces before it erases"
else
    bad "do_wipe_overlay must call quiesce_overlay before flash_eraseall"
fi
if sed -n '/^quiesce_overlay()/,/^}/p' "$SRC" | grep -q '/proc/mounts'; then
    ok "quiesce_overlay looks the mount point up (it moves with the pivot)"
else
    bad "quiesce_overlay must read /proc/mounts; the overlay is at /mnt/overlay after the pivot"
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
# do_update_kernel is the one that depends on the layout. A dedicated kernel
# partition is not mounted, so marking it would cost -x its only real use; but
# where there is none, kernel_device is the combined "firmware" partition, which
# overlaps the running rootfs -- so the mark has to be conditional, never absent
# and never unconditional.
kbody=$(sed -n '/^do_update_kernel()/,/^}/p' "$SRC")
if printf '%s\n' "$kbody" | grep -q 'mark_live_flash_dirty' &&
    printf '%s\n' "$kbody" | grep -q 'mark_flash_touched' &&
    printf '%s\n' "$kbody" | grep -q 'get_device "kernel"'; then
    ok "do_update_kernel marks live only when its target is the combined partition"
elif printf '%s\n' "$kbody" | grep -q 'mark_live_flash_dirty'; then
    bad "do_update_kernel marks dirty unconditionally -- a dedicated kernel partition is not mounted, and -x loses its only real use"
else
    bad "do_update_kernel never marks live -- on a layout with no kernel partition it writes the running rootfs and -x would be honoured after it"
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

# --- the boot-side half of the breadcrumb -----------------------------------
# free_resources tells a collector "logging stops here until this camera
# returns". Something has to say it returned, or the sentence has no end and a
# reader cannot tell a camera that came back from one that did not. These are
# tree-level assertions on that counterpart, not on $SRC.
BOOTMSG=${BOOTMSG:-general/overlay/etc/init.d/S41bootmsg}
if [ -x "$BOOTMSG" ]; then
    ok "the boot-side counterpart exists and is executable"
else
    bad "$BOOTMSG must exist and be executable, or rcS will not run it"
fi

# Same priority as the two lines it pairs with, or a collector filtering at
# >= warning gets the half that says the log stopped and not the half that
# says it came back.
if grep -q 'user\.warning' "$BOOTMSG" && grep -q 'user\.warning' "$SRC"; then
    ok "both halves of the breadcrumb log at user.warning"
else
    bad "the boot marker and sysupgrade's must share a priority, or one is filtered out"
fi

# Gated, so a camera that forwards nothing pays nothing.
if grep -q 'SYSLOG_REMOTE' "$BOOTMSG"; then
    ok "the boot marker is gated on SYSLOG_REMOTE"
else
    bad "the boot marker must be gated on SYSLOG_REMOTE; every camera would pay for it"
fi

# Backgrounded. It waits up to fifteen seconds for a DHCP lease, and doing that
# in line would hold up every later init script -- majestic, and the video with
# it -- on exactly the cameras that asked for forwarding.
if grep -qE '^\) &' "$BOOTMSG"; then
    ok "the boot marker waits for its address off the boot path"
else
    bad "the boot marker must background its wait, or it delays the boot it reports on"
fi

# A hostname destination is resolved once, at S01, before the network exists,
# and busybox retries only every 120 s (etc/default/syslogd says so). Anything
# sent inside that window is dropped however ready the path is, so the marker
# has to wait it out on a name -- confirmed on an hi3516av300, where the marker
# was present locally and absent at a hostname collector. Restarting syslogd to
# force a re-resolve is NOT the fix: its buffer is in RAM and logread loses the
# whole boot with it.
if grep -q 'sleep 125' "$BOOTMSG" && grep -q '\*\[!0-9\.\]\*' "$BOOTMSG"; then
    ok "the boot marker waits out the DNS window when the collector is a name"
else
    bad "a hostname collector drops everything for 120s; the marker must wait that out"
fi
if grep -q 'S01syslogd restart\|syslogd restart' "$BOOTMSG"; then
    bad "the boot marker must not restart syslogd; its RAM buffer is the boot's local log"
else
    ok "...without restarting syslogd and losing the in-RAM boot log"
fi

# --- the watchdog keeper ---------------------------------------------------
#
# majestic is the only thing on the image that pets the hardware watchdog (true
# on gk7205v200, hi3516av300 and t31 alike), and the flash window is exactly
# where it dies: it is demand-paged from the partition being erased and
# free_resources has already dropped the cache, so its next fault is SIGBUS. The
# driver leaves the dog armed on that close -- measured on gk7205v200, the SoC
# hard-resets 297-307 s later, which lands inside the write on a slow enough
# flash and leaves the rootfs part-written.
#
# Two properties hold the fix together, and neither shows up in the flash log.
kf=$(awk '/^flash_and_reboot\(\)/,/^}/' "$SRC")
printf '%s\n' "$kf" | grep -q 'watchdog_keep &' \
    && ok "the flash phase arms the watchdog keeper" \
    || bad "flash_and_reboot must start watchdog_keep"

# Outside the pivot the keeper's own `sleep` would be one more exec off the
# partition being erased, so it must not arm there.
printf '%s\n' "$kf" | grep -q '_ramfs_phase' \
    && ok "...only inside the pivot, where its sleep lives in RAM" \
    || bad "the keeper must be gated on _ramfs_phase"

# And it must never claim a device nobody was petting. On gk7205v200 the kernel
# has no CONFIG_WATCHDOG at all and open_wdt.ko feeds the dog while userspace
# holds no fd, so opening it on a camera whose owner turned the watchdog off
# would CREATE the unfed fuse this exists to prevent.
printf '%s\n' "$kf" | grep -q 'wdog_userspace_owned' \
    && ok "...and only when a userspace owner was seen before the pivot" \
    || bad "the keeper must be gated on wdog_userspace_owned"

grep -qE '^	export .*\bwdog_userspace_owned\b' "$SRC" \
    && ok "the owner verdict survives the re-exec (phase 2 cannot rescan)" \
    || bad "wdog_userspace_owned must be exported into the ramfs phase"

awk '/^watchdog_owner\(\)/,/^}/' "$SRC" | grep -q '\[ -c "\$WDOG" \]' \
    && ok "watchdog_owner asks whether there is a watchdog at all first" \
    || bad "watchdog_owner must check for the device before scanning"

# `-ef` is a test builtin in both busybox ash and dash, so the scan costs no
# forks; readlink cost one per open fd.
awk '/^watchdog_owner\(\)/,/^}/' "$SRC" | grep -q -- '-ef' \
    && ok "...and finds the holder without forking per descriptor" \
    || bad "watchdog_owner should compare with -ef rather than fork readlink"

# And it must not probe by opening: `exec` is a special builtin, so an open the
# kernel refuses -- which is exactly what it gets while the owner is alive --
# takes the keeper down without a word. Measured on hardware: probing that way
# killed the keeper before the owner it was waiting for had died.
kk=$(awk '/^watchdog_keep\(\)/,/^}/' "$SRC")
kow=$(printf '%s\n' "$kk" | grep -n 'watchdog_owner' | head -1 | cut -d: -f1)
kex=$(printf '%s\n' "$kk" | grep -n 'exec 9>' | head -1 | cut -d: -f1)
if [ -n "$kow" ] && [ -n "$kex" ] && [ "$kow" -lt "$kex" ]; then
    ok "the keeper asks who holds the device before it opens it"
else
    bad "watchdog_keep must test ownership before `exec 9>` (owner=$kow exec=$kex)"
fi

if awk '/^watchdog_keep\(\)/,/^}/' "$SRC" | grep -qF 'printf V'; then
    bad "the keeper must NOT magic-close: a reboot that cannot exec needs the dog armed"
else
    ok "the keeper never disarms the dog, so a wedged reboot is still rescued"
fi

rbw=$(awk '/^reboot_system\(\)/,/^}/' "$SRC")
relw=$(printf '%s\n' "$rbw" | grep -n 'WDOG_RELEASE' | head -1 | cut -d: -f1)
rbtw=$(printf '%s\n' "$rbw" | grep -n 'busybox reboot' | head -1 | cut -d: -f1)
if [ -n "$relw" ] && [ -n "$rbtw" ] && [ "$relw" -lt "$rbtw" ]; then
    ok "petting stops before the reboot, not after it"
else
    bad "reboot_system must release the watchdog before it reboots (rel=$relw reboot=$rbtw)"
fi

grep -q '^WDOG=${WDOG:-/dev/watchdog}' "$SRC" && grep -q '^WDOG_PROC=${WDOG_PROC:-/proc}' "$SRC" \
    && ok "both watchdog paths are overridable, so this suite never scans the real /proc" \
    || bad "WDOG and WDOG_PROC must be overridable"

# The short-rootfs refusal belongs on the pre-pivot path: inside the pivot a
# die() has to reboot, and a reboot with nothing written is the outcome die()
# goes out of its way to avoid.
awk '/^preflight_image_sizes\(\)/,/^}/' "$SRC" | grep -q 'check_rootfs_complete' \
    && ok "the incomplete-image check runs before the pivot" \
    || bad "preflight_image_sizes must call check_rootfs_complete"

echo
if [ "$fail" -eq 0 ]; then
    echo "All sysupgrade verification checks passed."
    exit 0
else
    echo "$fail check(s) failed."
    exit 1
fi
