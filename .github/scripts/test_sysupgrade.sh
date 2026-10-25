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

for t in xxd od timeout dash busybox; do
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
#  1. get_system_version() and get_system_platform() take their root as "$1" — "" for the running system
#     (which must be sandboxed) and the mountpoint for the candidate rootfs
#     (which must NOT be). Give it a default and hide the literal behind a
#     sentinel so rule 3 cannot touch it. Getting this wrong makes every
#     version read empty, which silently turns the same-version test green.
#  2. /tmp\b, before anything that inserts a /tmp path of its own.
sed -e 's|"$1/etc/os-release"|"${1:-@SB@}@OSRELEASE@"|g' \
    -e "s|/tmp\\b|@SB@/tmp|g" \
    -e "s|/etc/os-release|@SB@/etc/os-release|g" \
    -e "s|/proc/mtd|@SB@/proc/mtd|g" \
    -e "s|/proc/cmdline|@SB@/proc/cmdline|g" \
    -e "s|/proc/mounts|@SB@/proc/mounts|g" \
    -e "s|/proc/meminfo|@SB@/proc/meminfo|g" \
    -e "s|/proc/sys/vm/min_free_kbytes|@SB@/proc/min_free_kbytes|g" \
    -e "s|/proc/sys/vm/drop_caches|@SB@/tmp/drop_caches|g" \
    -e "s|/etc/init.d/|@SB@/etc/init.d/|g" \
    -e "s|/var/run/vigil-supervise.pid|@SB@/tmp/vigil-supervise.pid|g" \
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
# A stop can be given back as memory: STUB_MEM_AFTER_STOP is the MemAvailable
# it leaves, for the tests of stop_majestic_for_memory.
cat > "$SB/etc/init.d/S95majestic" <<'STUB'
#!/bin/sh
# The supervisor will not start majestic while an upgrade holds the lock, so a
# start or restart made under it is logged as such.
if [ "$1" != stop ] && [ -f "$STUB_LOCK" ]; then
    echo "S95majestic $1 (under the upgrade lock)" >> "$FLASH_LOG"
else
    echo "S95majestic $1" >> "$FLASH_LOG"
fi
[ "$1" = stop ] && [ -n "$STUB_STOP_FAILS" ] && exit 1
# A stopped majestic holds no file any more, the watchdog included.
[ "$1" = stop ] && [ -n "$STUB_MAJESTIC_PID" ] && [ -n "$WDOG_PROC" ] &&
    rm -rf "$WDOG_PROC/$STUB_MAJESTIC_PID"
if [ "$1" = stop ] && [ "$STUB_MEM_AFTER_STOP" = unreadable ]; then
    : > "$STUB_MEMINFO"
elif [ "$1" = stop ] && [ -n "$STUB_MEM_AFTER_STOP" ]; then
    printf 'MemTotal: 131072 kB\nMemFree: %d kB\nMemAvailable: %d kB\n' \
        "$STUB_MEM_AFTER_STOP" "$STUB_MEM_AFTER_STOP" > "$STUB_MEMINFO"
fi
exit 0
STUB
chmod +x "$SB/etc/init.d/S95majestic"

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

# The camera's timeout, not the host's. They differ exactly where it matters:
# sysupgrade ignores TERM past its #2025 trap, and GNU timeout gives its child
# TERM back at default while busybox's does not, so the host's timeout let the
# mount-hang case pass against a bound that never fired on a camera.
# By absolute path: `busybox` on the run's PATH is the stub below. The program
# is handed over by absolute path too: a distro busybox prefers its own applets,
# so a bare `mount` would run the real one instead of the mount stub.
stub timeout "
opts=()
while :; do
    case \$1 in
        -s|-k) opts+=(\"\$1\" \"\$2\"); shift 2 ;;
        -*)    opts+=(\"\$1\"); shift ;;
        *)     break ;;
    esac
done
secs=\$1 prog=\$(command -v \"\$2\"); shift 2
exec $(command -v busybox) timeout \"\${opts[@]}\" \"\$secs\" \"\$prog\" \"\$@\""

stub ipcinfo    'case "$1" in -v) echo "${STUB_VENDOR:-sigmastar}";; -F) echo "${STUB_FLASH:-nor}";; esac'
# majestic is running when a test says so, and only then: the host's own
# pidof would otherwise decide.
stub pidof '[ "$1" = majestic ] && [ -n "$STUB_MAJESTIC_PID" ] && { echo "$STUB_MAJESTIC_PID"; exit 0; }; [ "$1" = vigild ] && [ -n "$STUB_VIGILD_PID" ] && { echo "$STUB_VIGILD_PID"; exit 0; }; exit 1'
# Into the flash log, not the host's syslog.
stub logger 'echo "logger $*" >> "$FLASH_LOG"'
# `upgrade` is the URL a builder profile writes on first boot; unset models
# the env that lost it, or never had it (#2484).
stub fw_printenv '
if [ "$2" = upgrade ]; then
    [ -n "${STUB_UPGRADE:-}" ] && { echo "$STUB_UPGRADE"; exit 0; }
    exit 1
fi
echo "${STUB_SOC:-ssc338q}"'
stub killall    'echo "killall $*" >> "$FLASH_LOG"; exit 0'
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
# majestic_api: POST http://127.0.0.1[:port]/api/v1/upgrade/{prepare,abort}.
# STUB_PREPARE / STUB_ABORT are the status codes majestic answers; the 404
# default is a majestic too old to have the endpoint, which is what every test
# that does not care about it gets.
for a in "$@"; do
    case "$a" in
    http://127.0.0.1*/api/v1/upgrade/*)
        action=${a##*/}
        echo "majestic_api $action" >> "$FLASH_LOG"
        if [ "$action" = prepare ]; then printf %s "${STUB_PREPARE:-404}"
        else printf %s "${STUB_ABORT:-404}"; fi
        exit 0 ;;
    esac
done
out=""; prev=""; ranged=0; head=0; w=0; url=""
for a in "$@"; do
    [ "$prev" = "-o" ] && out=$a
    [ "$a" = "-r" ] && ranged=1
    [ "$a" = "-sIL" ] && head=1
    [ "$a" = "-w" ] && w=1
    case "$a" in http*://*) url=$a ;; esac
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
if [ "$head" = "1" ] && [ "$w" = "1" ]; then
    # url_exists: the status alone. A -split package exists only where the
    # release publishes one (STUB_SPLIT_PUBLISHED); everything else does.
    case "$url" in
    *-split.tgz) [ -n "${STUB_SPLIT_PUBLISHED:-}" ] && printf 200 || printf 404 ;;
    *) printf 200 ;;
    esac
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
[ "$out" = "-" ] && [ -n "${STUB_DL_FILE:-}" ] && cat "$STUB_DL_FILE"
# fetch_manifest: the channel/build index, when a test supplies one.
case "$out" in *manifest.flat) [ -n "${STUB_MANIFEST:-}" ] && cp "$STUB_MANIFEST" "$out" ;; esac
exit "${STUB_CURL_RC:-0}"'
# check_sdcard re-reads `mount` after every umount, so a static pair of stubs
# would spin forever: the unmount has to actually change what mount reports.
# $SDMOUNTS is what a bare `mount` prints; empty is the default, which is what
# every test that does not care about an SD card sees.
SDMOUNTS="$SB/tmp/sdmounts"
: > "$SDMOUNTS"
cat > "$SB/bin/umount" <<EOF
#!/bin/bash
# STUB_UMOUNT_BUSY: a card something still has open -- EBUSY, mount unchanged.
[ -n "\${STUB_UMOUNT_BUSY:-}" ] && exit 1
# STUB_UMOUNT_HIDES: the card's files go with it, as they do on a camera -- the
# directory left behind is an empty mount point.
[ -n "\${STUB_UMOUNT_HIDES:-}" ] && [ -d "\$1" ] && mv "\$1" "\$1.unmounted"
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
# enter_ramfs proves the staged root can run before it moves anything; an
# unprivileged host cannot chroot, so that probe answers STUB_CHROOT_RC (#2605).
stub chroot     'echo "chroot $*" >> "$FLASH_LOG"; exit ${STUB_CHROOT_RC:-0}'
stub losetup    'case "$1" in -f) echo /dev/loop0;; -d) echo "losetup -d $2" >> "$FLASH_LOG";; esac; exit 0'

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
    # UBI volume writes, the PID 1 hand-off and the stage-2 unmount: logged
    # like flashcp, so a test can assert what reached which volume, in order.
    # STUB_UBI_SLOW reads the image on fd 0 a slice at a time, the way the
    # real one moves it there and reads it a LEB at a time, so the progress
    # ubi_update reads off /proc/<pid>/fdinfo/0 has something to see.
    ubiupdatevol)           echo "ubiupdatevol $*" >> "$FLASH_LOG"
                            if [ -n "$STUB_UBI_SLOW" ] && [ -f "$2" ]; then
                                exec 0< "$2"
                                while [ "$(dd bs=1024 count=1 2>/dev/null | wc -c)" -gt 0 ]; do sleep 0.4; done
                            fi
                            [ "1" = "$STUB_FLASHCP_FAIL" ] && exit 1 ;;
    # follow_start: the console tail and the log server, logged, not run.
    tail|nc)                echo "$applet $*" >> "$FLASH_LOG" ;;
    kill|ubirmvol|ubirsvol|ubimkvol)
                            echo "$applet $*" >> "$FLASH_LOG" ;;
    umount)                 echo "$applet $*" >> "$FLASH_LOG"
                            case "$*" in *overlay.old*) [ "1" = "$STUB_OVL_UMOUNT_FAIL" ] && exit 1 ;; esac
                            exit "${STUB_BB_UMOUNT_RC:-0}" ;;
    # The applet list enter_ramfs links into the RAM root, and the RAM-root
    # reservation counts. Silent unless a test asks for one.
    --list)                 i=0
                            while [ "$i" -lt "${STUB_BB_APPLETS:-0}" ]; do
                                echo "applet$i"; i=$((i + 1))
                            done ;;
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
# rewrite_ubifs mounts rootfs_data read-only to copy the settings out, then
# the new, empty volume read-write to put them back. The copy finds a marker;
# the new volume starts empty, so the marker is there afterwards only if the
# settings really went round through the backup.
case " $* " in
    *" -t ubifs "*)
        case " $* " in
            *" ro "*) [ "1" = "$STUB_UBIFS_RO_FAIL" ] && exit 255
                      echo "mount ubifs ro $target" >> "$FLASH_LOG"
                      mkdir -p "$target/etc"; echo settings > "$target/etc/marker" ;;
            *)        echo "mount ubifs rw $target" >> "$FLASH_LOG"
                      rm -rf "$target"; mkdir -p "$target" ;;
        esac
        exit 0 ;;
    # On the split layout the handed-off RAM phase mounts the jffs2 overlay
    # read-only to copy the settings out, since its own mount has gone
    # with the old root. It finds the marker the camera had.
    # The files come from STUB_JFFS2_DIR/<device name>, so a mount of any
    # other device finds nothing and fails, as a wrong partition would.
    *" -t jffs2 "*)
        [ "1" = "$STUB_JFFS2_RO_FAIL" ] && exit 255
        dev=${@: -2:1}; src=$STUB_JFFS2_DIR/${dev##*/}
        [ -d "$src" ] || exit 255
        echo "mount jffs2 ro $dev $target" >> "$FLASH_LOG"
        mkdir -p "$target" && cp -a "$src/." "$target/"
        exit 0 ;;
esac
case "${STUB_MOUNT:-ok}" in
    ok)
        mkdir -p "$target/etc"
        echo "GITHUB_VERSION=${STUB_IMG_VERSION:-2026.07.11}" > "$target/etc/os-release"
        [ -n "${STUB_IMG_BUILD_ID:-}" ] &&
            echo "BUILD_ID=$STUB_IMG_BUILD_ID" >> "$target/etc/os-release"
        [ -n "${STUB_IMG_PLATFORM:-}" ] &&
            echo "BUILD_PLATFORM=$STUB_IMG_PLATFORM" >> "$target/etc/os-release"
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

# set_platform <BUILD_PLATFORM> [BUILD_OPTION]: the build the camera runs.
set_platform() {
    cat > "$SB/etc/os-release" <<EOF
BUILD_PLATFORM=$1
BUILD_OPTION=${2:-lite}
GITHUB_VERSION=2026.06.01
BUILD_ID=nightly-20260601-aaaaaaa
EOF
}
set_platform ssc338q_lite

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

# A FIT whose root description names a SoC, as the NAND FIT's does
# ("OpenIPC <soc>", board/<family>/nand-fit.its): fit_soc's witness.
make_fit_soc() {
    printf '\xd0\x0d\xfe\xed' > "$1"
    dd if=/dev/zero bs=1 count=60 >> "$1" 2>/dev/null
    printf 'description\0OpenIPC %s\0' "$2" >> "$1"
    dd if=/dev/zero bs=1 count=64 >> "$1" 2>/dev/null
}

# set_ubi [rootfs_reserved_ebs]: a UBI device with kernel/rootfs/rootfs_data
# volumes, as /sys/class/ubi shows them: 126976-byte LEBs (2 KiB pages,
# 128 KiB blocks). The rootfs volume is 2 LEBs by default -- room for the
# fixtures below and not much more, so a size test has an edge to cross.
# $2 is the volume list, $3 the overlay's LEBs (the ubifs layout's rootfs can
# grow into them, so its size tests need them to be real).
set_ubi() {
    local u="$SB/sys/class/ubi" i name ebs
    rm -rf "$u"
    i=0
    for name in ${2:-kernel rootfs rootfs_data}; do
        mkdir -p "$u/ubi0_$i"
        echo "$name" > "$u/ubi0_$i/name"
        echo 126976 > "$u/ubi0_$i/usable_eb_size"
        ebs=2; [ "$name" = rootfs ] && ebs=${1:-2}
        [ "$name" = rootfs_data ] && ebs=${3:-2}
        echo "$ebs" > "$u/ubi0_$i/reserved_ebs"
        i=$((i + 1))
    done
}

# A UBIFS image: superblock node magic at 0 (0x06101831, little-endian), node
# type 6 (superblock) at 20, and the LEB size it was made for at 36 -- the three
# fields sysupgrade reads. $2 is that LEB size (default: the volume's).
make_ubifs() {
    local leb=${2:-126976}
    printf '\x31\x18\x10\x06' > "$1"
    dd if=/dev/zero bs=1 count=16 >> "$1" 2>/dev/null         # 4..19
    printf '\x06' >> "$1"                                     # 20: node type
    dd if=/dev/zero bs=1 count=15 >> "$1" 2>/dev/null         # 21..35
    printf %b "$(printf '\\x%02x\\x%02x\\x%02x\\x%02x' \
        $((leb & 255)) $(((leb >> 8) & 255)) $(((leb >> 16) & 255)) $(((leb >> 24) & 255)))" >> "$1"
    dd if=/dev/zero bs=1k count=8 >> "$1" 2>/dev/null
}

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

# A FIT and a squashfs in one blob, the squashfs 64K after the FIT: the shape
# hi3516cv6xx and hi3519dv500 once shipped as firmware.bin. Not a split-layout
# image (that is a uImage, then the squashfs), so sysupgrade refuses it.
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
        STUB_CHROOT_RC="${STUB_CHROOT_RC:-0}" \
        STUB_MOUNT="${STUB_MOUNT:-ok}" STUB_VENDOR="${STUB_VENDOR:-sigmastar}" \
        STUB_SOC="${STUB_SOC:-ssc338q}" \
        STUB_IMG_SOC="${STUB_IMG_SOC:-ssc338q}" \
        STUB_IMG_VERSION="${STUB_IMG_VERSION:-2026.07.11}" \
        STUB_IMG_PLATFORM="${STUB_IMG_PLATFORM:-}" \
        STUB_IMG_BUILD_ID="${STUB_IMG_BUILD_ID:-}" \
        HISTORY_FILE="$SB/etc/sysupgrade.history" \
        STUB_UPGRADE="${STUB_UPGRADE:-}" \
        STUB_DL_FILE="${STUB_DL_FILE:-}" \
        STUB_FLASHCP_FAIL="${STUB_FLASHCP_FAIL:-0}" \
        STUB_FLASHCP_FAIL_DEV="${STUB_FLASHCP_FAIL_DEV:-}" \
        STUB_REMOUNT_RC="${STUB_REMOUNT_RC:-0}" \
        STUB_CURL_RC="${STUB_CURL_RC:-0}" \
        STUB_PREPARE="${STUB_PREPARE:-}" STUB_ABORT="${STUB_ABORT:-}" \
        STUB_UMOUNT_BUSY="${STUB_UMOUNT_BUSY:-}" \
        STUB_UMOUNT_HIDES="${STUB_UMOUNT_HIDES:-}" \
        STUB_DL_BYTES="${STUB_DL_BYTES:-}" \
        STUB_ISIZE="${STUB_ISIZE:-}" \
        STUB_RANGE_IGNORED="${STUB_RANGE_IGNORED:-}" \
        STUB_SPLIT_PUBLISHED="${STUB_SPLIT_PUBLISHED:-}" \
        UNPACK_RESERVE_KB="${UNPACK_RESERVE_KB:-512}" \
        STUB_MEM_AFTER_STOP="${STUB_MEM_AFTER_STOP:-}" STUB_MEMINFO="$SB/proc/meminfo" \
        STUB_LOCK="$SB/tmp/sysupgrade.lock" \
        STUB_STOP_FAILS="${STUB_STOP_FAILS:-}" \
        STUB_MAJESTIC_PID="${STUB_MAJESTIC_PID:-}" \
        STUB_VIGILD_PID="${STUB_VIGILD_PID:-}" STUB_VIGIL_STOP_FAILS="${STUB_VIGIL_STOP_FAILS:-}" \
        FLASH_RAM_FLOOR_KB="${FLASH_RAM_FLOOR_KB:-2048}" \
        UBI_SYS="$SB/sys/class/ubi" INITTAB="$SB/etc/inittab" handoff_wait=0 \
        FOLLOW_LOG="${FOLLOW_LOG:-}" STUB_UBI_SLOW="${STUB_UBI_SLOW:-}" \
        ${RUN_ENV:-} \
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
    unset STUB_UPGRADE STUB_IMG_PLATFORM STUB_IMG_BUILD_ID STUB_DL_FILE
    rm -f "$SB/etc/sysupgrade.history"
    set_platform ssc338q_lite
    unset STUB_FLASHCP_FAIL STUB_FLASHCP_FAIL_DEV STUB_PIVOT_RC STUB_REMOUNT_RC STUB_CURL_RC
    unset STUB_DL_BYTES STUB_ISIZE STUB_RANGE_IGNORED UNPACK_RESERVE_KB FLASH_RAM_FLOOR_KB
    unset STUB_MEM_AFTER_STOP STUB_MAJESTIC_PID STUB_STOP_FAILS WDOG WDOG_PROC
    unset STUB_VIGILD_PID STUB_VIGIL_STOP_FAILS
    unset STUB_PREPARE STUB_ABORT STUB_UMOUNT_BUSY STUB_UMOUNT_HIDES RUN_ENV FOLLOW_LOG STUB_UBI_SLOW
    rm -rf "$SB/sys"
    printf '::sysinit:/etc/init.d/rcS\n::shutdown:/bin/umount -a -f\n::restart:/sbin/init\n' > "$SB/etc/inittab"
    echo 540 > "$SB/proc/min_free_kbytes"
    rm -f "$SB"/tmp/*.gk7205v500
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

# --- the staged archive is not kept ----------------------------------------
# The WebUI's "install from a file" route uploads the .tgz into /tmp and then
# points --archive at it, so from the unpack onwards /tmp holds the image twice
# over -- on a 64 MB SigmaStar that is most of the tmpfs, and the flash phase
# still wants room for the dd split and the verify mount
# (OpenIPC/majestic-webui#474). $SB/tmp IS /tmp here: the harness rewrites the
# literal in the script under test, so the guard fires exactly as it would on a
# camera.
reset_env
make_archive "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
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
make_archive "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
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
make_archive "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
mv "$SB/tmp/fw.tgz" "$SB/keep/fw.tgz"
run -z --archive="$SB/tmp/../keep/fw.tgz"
if [ "$RC" -eq 0 ] && [ -f "$SB/keep/fw.tgz" ]; then
    ok "an archive reached through /tmp/.. is still the caller's"
else
    bad "a /tmp/.. alias must not delete an outside archive, rc=$RC present=$([ -f "$SB/keep/fw.tgz" ] && echo yes || echo no)"
fi

# ...and an archive on the SD card is unpacked before check_sdcard takes the
# card away. Unmounting first left "File ... not found" every time: the most
# natural place to keep an offline image was the one place it could not be read
# from.
reset_env
mkdir -p "$SB/mnt/mmcblk0p1"
make_archive "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
rm -f "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
mv "$SB/tmp/fw.tgz" "$SB/mnt/mmcblk0p1/fw.tgz"
echo "/dev/mmcblk0p1 on $SB/mnt/mmcblk0p1 type vfat (rw,relatime)" > "$SDMOUNTS"
STUB_UMOUNT_HIDES=1
run -z --archive="$SB/mnt/mmcblk0p1/fw.tgz"
u=$(at "Local archive unpacked"); c=$(at "Unmounting SD card")
if [ "$RC" -eq 0 ] && [ -n "$u" ] && [ -n "$c" ] && [ "$u" -lt "$c" ] &&
    [ -f "$SB/mnt/mmcblk0p1.unmounted/fw.tgz" ]; then
    ok "an archive on the SD card is unpacked before the card is unmounted, and kept"
else
    bad "archive on the SD card, rc=$RC unpacked@${u:-none} unmount@${c:-none} out='$OUT'"
fi
rm -rf "$SB/mnt"

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
# A kernel that reports neither MemAvailable nor the fields to estimate it
# leaves no RAM figure, so the cap stands: nothing says it may be raised.
reset_env
make_archive "$K" "$R"
stub df 'echo "Filesystem 1K-blocks Used Available Use% Mounted on"; echo "tmpfs 13564 13560 4 99% /tmp"'
printf 'MemTotal: 131072 kB\nMemFree: 65536 kB\n' > "$SB/proc/meminfo"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "No room to unpack" &&
    ! grep -q "remount.*size=" "$SB/tmp/flash.log"; then
    ok "an unpack that cannot fit is refused before it truncates an image"
else
    bad "full /tmp -> expected a refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
rm -f "$SB/bin/df"

# ...but a cap is not memory. tmpfs takes pages only as they are written, and
# defaults to half of RAM: on an XM510 at mem=18M that is 7804 KB against the
# 8341 KB an archive and its images need together, so no archive could be
# installed there at all (#2618). Where RAM was measured to hold the unpack,
# the cap is raised for the run instead.
reset_env
make_archive "$K" "$R"
stub df 'echo "Filesystem 1K-blocks Used Available Use% Mounted on"; echo "tmpfs 13564 13560 4 99% /tmp"'
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && rebooted &&
    grep -q "^remount -o remount,size=[0-9]*k .*/tmp$" "$SB/tmp/flash.log" &&
    printf '%s' "$OUT" | grep -q "Raised the .*/tmp cap from 13564 KB to [0-9]* KB"; then
    ok "a /tmp short only by its cap is raised when RAM can back it (#2618)"
else
    bad "/tmp cap with RAM to spare -> expected the cap raised and a flash, rc=$RC out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi
rm -f "$SB/bin/df"

# An archive in /tmp that cannot be unpacked beside majestic gets majestic
# stopped by check_unpack_ram anyway; asking it into upgrade mode first is
# what an XM510 at mem=18M could not afford -- majestic, a minute old, was
# OOM-killed during prepare and the run died with it (#2618). So it is
# stopped up front instead, and never asked to prepare.
reset_env
make_archive "$K" "$R"
set_meminfo 100           # the sandbox archive unpacks to about 20 KB
STUB_MAJESTIC_PID=901 STUB_MEM_AFTER_STOP=60000
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && grep -q "S95majestic stop" "$SB/tmp/flash.log" &&
    ! grep -q "majestic_api prepare" "$SB/tmp/flash.log" &&
    printf '%s' "$OUT" | grep -q "Short of memory by [0-9]* KB with majestic running"; then
    ok "an archive that cannot unpack beside majestic stops it before prepare (#2618)"
else
    bad "short archive -> expected majestic stopped instead of prepared, rc=$RC out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi
# majestic owning the watchdog does not keep it from that stop: its own stop
# releases the device with the magic close. But the ownership is recorded
# first, so the RAM-phase keeper still arms in case a driver ignores it.
mkdir -p "$SB/wdproc/901/fd"
ln -sf /dev/null "$SB/wdproc/901/fd/3"
reset_env
make_archive "$K" "$R"
set_meminfo 100
STUB_MAJESTIC_PID=901 STUB_MEM_AFTER_STOP=60000
WDOG=/dev/null WDOG_PROC="$SB/wdproc"
run -z --archive="$SB/tmp/fw.tgz"
if grep -q "S95majestic stop" "$SB/tmp/flash.log" && ! grep -q "majestic_api prepare" "$SB/tmp/flash.log" &&
    printf '%s' "$OUT" | grep -q "majestic fed it and was stopped for memory; taken over at the flash"; then
    ok "an owning majestic is stopped early too, and its ownership still arms the keeper"
else
    bad "owning majestic, short archive -> expected an early stop with the keeper armed, rc=$RC out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi
rm -rf "$SB/wdproc"

# A stop that does not happen falls back to prepare, as if nothing were short.
reset_env
make_archive "$K" "$R"
set_meminfo 100
STUB_MAJESTIC_PID=901 STUB_STOP_FAILS=1 STUB_MEM_AFTER_STOP=60000
run -z --archive="$SB/tmp/fw.tgz"
if grep -q "majestic_api prepare" "$SB/tmp/flash.log"; then
    ok "...and a majestic that will not stop is still asked to prepare"
else
    bad "failed early stop -> expected the prepare fallback, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# ...and one with room leaves majestic to prepare, as before.
reset_env
make_archive "$K" "$R"
STUB_MAJESTIC_PID=901
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && grep -q "majestic_api prepare" "$SB/tmp/flash.log" &&
    ! grep -q "S95majestic stop" "$SB/tmp/flash.log"; then
    ok "...while an archive with room leaves majestic to prepare"
else
    bad "archive with room -> expected prepare, not a stop, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# A kernel before 3.14 has no MemAvailable. It is estimated the way
# si_mem_available() computes it -- here from what a lab XM510 (3.0.101)
# reported: free 1516 - low watermark 675 (540 min_free + a quarter), file
# cache 7684 - 675, reclaimable slab 904 - 452 = 8302 KB.
reset_env
printf 'MemTotal: 15608 kB\nMemFree: 1516 kB\nBuffers: 2232 kB\nCached: 5548 kB\nActive(file): 1936 kB\nInactive(file): 5748 kB\nShmem: 96 kB\nSReclaimable: 904 kB\n' > "$SB/proc/meminfo"
run -z --kernel="$K" --rootfs="$R"
if printf '%s' "$OUT" | grep -q "RAM left to flash from: 8302 KB"; then
    ok "no MemAvailable (kernel < 3.14): estimated as si_mem_available() would"
else
    bad "MemAvailable estimate -> expected 8302 KB, out='$(printf '%s' "$OUT" | grep -i 'RAM left')'"
fi
# ...and a meminfo missing a field it needs is still no opinion.
reset_env
printf 'MemTotal: 15608 kB\nMemFree: 1516 kB\nActive(file): 1936 kB\nInactive(file): 5748 kB\n' > "$SB/proc/meminfo"
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && ! printf '%s' "$OUT" | grep -q "RAM left to flash from"; then
    ok "...and a meminfo it cannot estimate from is no opinion, not a refusal"
else
    bad "partial meminfo -> expected no RAM figure and a flash, rc=$RC out='$OUT'"
fi

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

# majestic is most of what is missing, on either path: in upgrade mode it locks
# itself into RAM (11476 KB available on a gk7201v200, 18888 KB with it
# stopped). Short of memory, a run stops it and asks again rather than refuse.
reset_env
set_meminfo 8192
STUB_DL_BYTES=8691055
STUB_MAJESTIC_PID=901 STUB_MEM_AFTER_STOP=16000
run -z --web -k -r
if grep -q "S95majestic stop" "$SB/tmp/flash.log" &&
    ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack" &&
    printf '%s' "$OUT" | grep -q "stopping it, which gives back what it holds in RAM" &&
    printf '%s' "$OUT" | grep -q "This page loses the log here"; then
    ok "a WebUI run short of memory stops majestic and goes on to unpack"
else
    bad "short + majestic running -> expected majestic stopped and no refusal, rc=$RC out='$OUT'"
fi

reset_env
set_meminfo 8192
STUB_DL_BYTES=8691055
STUB_MAJESTIC_PID=901 STUB_MEM_AFTER_STOP=16000
run -z -k -r
if grep -q "S95majestic stop" "$SB/tmp/flash.log" &&
    ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack" &&
    ! printf '%s' "$OUT" | grep -q "This page loses the log"; then
    ok "...and so does a console run, which is not told about a page it has not got"
else
    bad "console run short + majestic running -> expected it stopped, rc=$RC out='$OUT'"
fi

# Still short with majestic stopped: refused as before, and majestic is put
# back, since the camera is staying up.
reset_env
set_meminfo 8192
STUB_DL_BYTES=8691055
STUB_MAJESTIC_PID=901 STUB_MEM_AFTER_STOP=8300
run -z --web -k -r
if [ "$RC" -ne 0 ] && nothing_wrote &&
    printf '%s' "$OUT" | grep -q "8300 KB free with majestic stopped" &&
    grep -qx "S95majestic start" "$SB/tmp/flash.log"; then
    ok "still short with majestic stopped -> refused, and majestic started again"
else
    bad "short even without majestic -> expected a refusal and a start, rc=$RC out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi
# After the lock is gone, or the supervisor declines and the camera is left
# with no majestic and no WebUI.
if ! grep -q "under the upgrade lock" "$SB/tmp/flash.log" &&
    [ "$(logged_at "S95majestic start")" -gt 0 ] 2>/dev/null; then
    ok "...after the upgrade lock is released, so the supervisor starts it"
else
    bad "majestic must be started after the lock is released, log='$(cat "$SB/tmp/flash.log")'"
fi
# The page lost the log before the refusal, so the camera's log carries it.
if grep -q "^logger .*upgrade refused, nothing was written: Not enough memory to unpack" "$SB/tmp/flash.log" &&
    ! printf '%s' "$OUT" | grep -q "the camera reboots on its own once"; then
    ok "...and the refusal goes to syslog, with no reboot promised before the retry"
else
    bad "a refusal after majestic was stopped must reach syslog, out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi

# A stop that timed out freed nothing and took the supervisor: no retry on
# memory that was never given back, and a restart, not a start, to bring the
# video back.
reset_env
set_meminfo 8192
STUB_DL_BYTES=8691055
STUB_MAJESTIC_PID=901 STUB_MEM_AFTER_STOP=16000 STUB_STOP_FAILS=1
run -z --web -k -r
if [ "$RC" -ne 0 ] && nothing_wrote &&
    printf '%s' "$OUT" | grep -q "majestic did not stop" &&
    printf '%s' "$OUT" | grep -q "Not enough memory to unpack" &&
    grep -qx "S95majestic restart" "$SB/tmp/flash.log"; then
    ok "a stop that fails is not retried on, and majestic is restarted"
else
    bad "failed stop -> expected a refusal and a restart, rc=$RC out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi

# A reading after the stop that will not parse is no reason to refuse, as the
# first is not.
reset_env
set_meminfo 8192
STUB_DL_BYTES=8691055
STUB_MAJESTIC_PID=901 STUB_MEM_AFTER_STOP=unreadable
run -z --web -k -r
if grep -q "S95majestic stop" "$SB/tmp/flash.log" &&
    ! printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "an unreadable /proc/meminfo after the stop is no opinion either"
else
    bad "unreadable meminfo after the stop -> expected no refusal, rc=$RC out='$OUT'"
fi

# Once majestic is stopped every way out must go through die(), which puts it
# and the lock back -- an interrupt too, until the flash phase ignores them.
awk '/^stop_majestic_for_memory\(\)/,/^}/' "$SRC" | grep -q "trap 'die .*' INT TERM HUP" \
    && ok "an interrupt after majestic is stopped goes through die()" \
    || bad "stop_majestic_for_memory must trap INT/TERM/HUP to die()"
# ...but never once the flash phase has taken them away: the floor is asked
# from inside enter_ramfs, and a die() from an interrupt there could reboot
# the camera halfway through the erase (#2024).
awk '/^stop_majestic_for_memory\(\)/,/^}/' "$SRC" | grep -B1 "trap 'die " | grep -q 'signals_ignored' \
    && grep -A1 "^trap '' HUP INT TERM PIPE" "$SRC" | grep -q '^signals_ignored=1' \
    && ok "...and leaves the flash phase's protection alone" \
    || bad "the die() trap must not be installed once signals are ignored for the flash"

# Never the majestic that pets the watchdog: nothing would pet it until the
# RAM phase. /dev/null stands in for the device -- a character device the
# sandbox can name -- and a fake /proc gives pid 4242 an fd on it.
mkdir -p "$SB/wdproc/4242/fd"
ln -sf /dev/null "$SB/wdproc/4242/fd/3"
reset_env
set_meminfo 8192
STUB_DL_BYTES=8691055
STUB_MAJESTIC_PID=4242 STUB_MEM_AFTER_STOP=16000
WDOG=/dev/null WDOG_PROC="$SB/wdproc"
run -z --web -k -r
if [ "$RC" -ne 0 ] && ! grep -q "S95majestic stop" "$SB/tmp/flash.log" &&
    printf '%s' "$OUT" | grep -q "Not enough memory to unpack"; then
    ok "...but a majestic holding the watchdog is left running, and the run refused"
else
    bad "majestic owns the watchdog -> expected it left alone, rc=$RC out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi
rm -rf "$SB/wdproc"

# --- #2484: a builder device profile must not be traded for the generic image
#
# A builder per-device build stamps BUILD_PLATFORM=<soc>_<variant>_<device> and
# is published as ${BUILD_PLATFORM}-nor.tgz; its profile writes that URL into
# the `upgrade` env var once, on first boot. With the var gone, a plain -k/-r
# used to key on BUILD_OPTION alone and fetch OpenIPC/firmware's
# openipc.<soc>-nor-lite.tgz -- same SoC stamp, fits the partitions, passes
# every check, and boots without the profile's WiFi driver or mtdparts.
# The memory refusal stops each run right after the URL is chosen, before
# anything is unpacked.
default_url_is() {
    local want=$1; shift
    reset_env
    set_meminfo 8192
    STUB_DL_BYTES=8691055
    "$@"
    run -z -k -r
    if printf '%s' "$OUT" | grep -qF "Download from $want" && nothing_wrote; then
        ok "default URL -> $want"
    else
        bad "expected 'Download from $want', rc=$RC out='$OUT'"
    fi
}
B=https://github.com/OpenIPC/builder/releases/download/latest
F=https://github.com/OpenIPC/firmware/releases/download/latest
default_url_is "$B/ssc338q_lite_acme-cam1-nor.tgz" set_platform ssc338q_lite_acme-cam1
default_url_is "$F/openipc.ssc338q-nor-lite.tgz"   set_platform ssc338q_lite
default_url_is "$F/openipc.ssc338q-nor-ultimate.tgz" set_platform ssc338q_ultimate ultimate
default_url_is "$B/openipc.ssc338q-nor-fpv.tgz"    set_platform ssc338q_fpv fpv
default_url_is "https://mirror.example/x.tgz" eval 'set_platform ssc338q_lite_acme-cam1; STUB_UPGRADE=https://mirror.example/x.tgz'

# Whatever the URL, the rootfs itself says what it was built as. A camera on a
# device profile takes only an image for the same device.
reset_env
set_platform ssc338q_lite_acme-cam1
STUB_IMG_PLATFORM=ssc338q_lite
run -z --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "Wrong platform"; then
    ok "device profile + generic rootfs -> refused, nothing written"
else
    bad "device profile + generic rootfs -> expected refusal, rc=$RC out='$OUT'"
fi
printf '%s' "$OUT" | grep -q -- "--force_soc" \
    && ok "...and names the option that overrides it" \
    || bad "platform refusal should advise --force_soc, out='$OUT'"
# Without the ramfs die() does not reboot, so a refusal that left the candidate
# mounted would hand the next attempt a taken loop device.
grep -q "losetup -d" "$SB/tmp/flash.log" \
    && ok "...after releasing the candidate's loop device" \
    || bad "platform refusal left the loop device attached, log='$(cat "$SB/tmp/flash.log")'"

# An image from before BUILD_PLATFORM was stamped (1.0.51) has nothing to
# compare, and passes as it always did.
reset_env
set_platform ssc338q_lite_acme-cam1
run -z --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && printf '%s' "$OUT" | grep -q "no BUILD_PLATFORM"; then
    ok "device profile + unstamped rootfs -> flashed, and says it could not compare"
else
    bad "device profile + unstamped rootfs -> expected to proceed with a note, rc=$RC out='$OUT'"
fi

# Unmountable (the running kernel lacks the new image's decompressor): the
# artifact name is the only evidence left. It pins the SoC, which is enough on
# a stock camera; on a device profile only a name carrying the device is.
reset_env
set_platform ssc338q_lite_acme-cam1
make_fit "$SB/tmp/uImage.ssc338q"
make_archive "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
STUB_MOUNT=fail
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "device profile"; then
    ok "device profile + unmountable archive -> refused, nothing written"
else
    bad "device profile + unmountable archive -> expected refusal, rc=$RC out='$OUT'"
fi

reset_env
set_platform ssc338q_lite_acme-cam1
make_fit "$SB/tmp/uImage.ssc338q"
make_archive "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
mkdir -p "$SB/keep" && mv "$SB/tmp/fw.tgz" "$SB/keep/dl.tgz"
rm -f "$SB"/tmp/*.ssc338q
STUB_DL_FILE="$SB/keep/dl.tgz"
STUB_MOUNT=fail
run -z -k -r
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3; then
    ok "device profile + unmountable image fetched under the device's name -> proceeds"
else
    bad "the device's own artifact should not be refused, rc=$RC out='$OUT'"
fi

reset_env
set_platform ssc338q_lite_acme-cam1
make_fit "$SB/tmp/uImage.ssc338q"
make_archive "$SB/tmp/uImage.ssc338q" "$SB/tmp/rootfs.squashfs.ssc338q"
mv "$SB/tmp/fw.tgz" "$SB/keep/dl.tgz"
rm -f "$SB"/tmp/*.ssc338q
STUB_DL_FILE="$SB/keep/dl.tgz"
STUB_UPGRADE=https://github.com/OpenIPC/firmware/releases/download/latest/openipc.ssc338q-nor-lite.tgz
STUB_MOUNT=fail
run -z -k -r
if [ "$RC" -ne 0 ] && nothing_wrote; then
    ok "device profile + unmountable image from a generic URL -> refused"
else
    bad "a generic URL is no proof of the device, rc=$RC out='$OUT'"
fi
rm -rf "$SB/keep"

reset_env
set_platform ssc338q_lite_acme-cam1
STUB_IMG_PLATFORM=ssc338q_lite
run -z --force_soc --rootfs="$R"
[ "$RC" -eq 0 ] && flashed /dev/mtd3 \
    && ok "device profile + generic rootfs + --force_soc -> flashed" \
    || bad "--force_soc should override the platform check, rc=$RC out='$OUT'"

# The same device's own image is what a profile camera upgrades to.
reset_env
set_platform ssc338q_lite_acme-cam1
STUB_IMG_PLATFORM=ssc338q_lite_acme-cam1
run -z --rootfs="$R"
[ "$RC" -eq 0 ] && flashed /dev/mtd3 \
    && ok "device profile + same device's rootfs -> flashed" \
    || bad "same device should pass, rc=$RC out='$OUT'"

# Moving onto a profile, and between stock variants, loses nothing that makes
# the camera reachable, so neither is the platform check's business.
reset_env
STUB_IMG_PLATFORM=ssc338q_lite_acme-cam1
run -z --rootfs="$R"
[ "$RC" -eq 0 ] && flashed /dev/mtd3 \
    && ok "generic build + device rootfs -> flashed (onboarding)" \
    || bad "generic -> device should pass, rc=$RC out='$OUT'"

reset_env
STUB_IMG_PLATFORM=ssc338q_ultimate
run -z --rootfs="$R"
[ "$RC" -eq 0 ] && flashed /dev/mtd3 \
    && ok "lite build + ultimate rootfs -> flashed" \
    || bad "lite -> ultimate should pass, rc=$RC out='$OUT'"

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

# A uClibc image has no ldd, so it staged busybox without a loader and the
# re-exec into the RAM root died "not found" with nothing left to fall back to
# (#2605). The staged root is now made to run something first; when it cannot,
# that is a refusal to pivot, not a reboot.
reset_env
STUB_CHROOT_RC=1
STUB_PIVOT_RC=0
run -z --kernel="$K" --rootfs="$R"
if grep -q "^chroot .*/ram .*/busybox true" "$SB/tmp/flash.log" &&
    ! grep -q "pivot_root" "$SB/tmp/flash.log" && flashed /dev/mtd3 && rebooted; then
    ok "a staged root that cannot run is not pivoted into; the flash goes on in place"
else
    bad "unrunnable ramfs -> expected no pivot and an in-place flash, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi
STUB_CHROOT_RC=0
STUB_PIVOT_RC=1

# Which libraries to stage, read from the ELF when there is no ldd to ask.
# Exercised on a fixture rather than the host's busybox, whose libraries live
# wherever the host distribution keeps them.
LIBFX="$SB/libfx"
mkdir -p "$LIBFX/lib" "$LIBFX/usr/lib"
: > "$LIBFX/lib/ld-uClibc-1.0.45.so"
ln -sf ld-uClibc-1.0.45.so "$LIBFX/lib/ld-uClibc.so.0"
: > "$LIBFX/lib/libuClibc-1.0.45.so"
ln -sf libuClibc-1.0.45.so "$LIBFX/lib/libc.so.0"
printf 'libgcc_s.so.1\n' > "$LIBFX/usr/lib/libgcc_s.so.1"
# What `strings` sees in a uClibc busybox: PT_INTERP, then .dynstr. libm.so.0
# is named but absent, standing in for anything that only looks like a library.
printf '\177ELF\n%s\nlibc.so.0\nlibm.so.0\nbusybox: applet not found\n' \
    "$LIBFX/lib/ld-uClibc.so.0" > "$LIBFX/busybox"
# ...and the C library itself names libgcc_s, which it dlopens for unwinding.
printf 'libgcc_s.so.1\n' >> "$LIBFX/lib/libuClibc-1.0.45.so"
libs=$( exec 3>/dev/null; ELF_LIB_DIRS="$LIBFX/lib $LIBFX/usr/lib"
    eval "$(awk '/^elf_libs\(\)/,/^}/' "$SRC")"
    for f in $(elf_libs "$LIBFX/busybox"); do echo "$f"; elf_libs "$f"; done | sort -u )
if printf '%s\n' "$libs" | grep -qx "$LIBFX/lib/ld-uClibc.so.0" &&
    printf '%s\n' "$libs" | grep -qx "$LIBFX/lib/libc.so.0" &&
    printf '%s\n' "$libs" | grep -qx "$LIBFX/usr/lib/libgcc_s.so.1" &&
    ! printf '%s\n' "$libs" | grep -q "libm"; then
    ok "without ldd, the loader and the libraries are read out of the ELF (#2605)"
else
    bad "elf_libs on a uClibc-shaped busybox gave: $(printf '%s ' $libs)"
fi
# busybox_libs keeps asking ldd first, and only an empty answer -- no ldd, or
# one that fails -- sends it to the ELF.
libs=$( exec 3>/dev/null; PATH="$SB/nolddbin:$PATH"; mkdir -p "$SB/nolddbin"
    printf '#!/bin/sh\nexit 1\n' > "$SB/nolddbin/ldd"; chmod +x "$SB/nolddbin/ldd"
    eval "$(awk '/^busybox_libs\(\)/,/^}/' "$SRC")"
    elf_libs() { [ "$1" = /bin/busybox ] && echo /lib/ld-fallback.so.1; }
    busybox_libs )
[ "$libs" = "/lib/ld-fallback.so.1" ] \
    && ok "an ldd that answers nothing falls back to reading the ELF" \
    || bad "busybox_libs with a silent ldd gave '$libs'"

# Where a local image is after the pivot: the old root sits at /mnt, so an SD
# card at /mnt/mmcblk0p1 is /mnt/mnt/mmcblk0p1 from inside it (#2605). /tmp
# alone is carried across whole.
PAP_FN=$(awk '/^path_after_pivot\(\)/,/^}/' "$SRC")
pap() ( exec 3>/dev/null; cd "${2:-/}"; eval "$PAP_FN"; path_after_pivot "$1" )
mkdir -p "$SB/rel"
if [ "$(pap /tmp/rootfs.squashfs.gm8135)" = "/tmp/rootfs.squashfs.gm8135" ] &&
    [ "$(pap /mnt/mmcblk0p1/rootfs.squashfs.gm8135)" = "/mnt/mnt/mmcblk0p1/rootfs.squashfs.gm8135" ] &&
    [ "$(pap /root/uImage)" = "/mnt/root/uImage" ] &&
    [ -z "$(pap "")" ]; then
    ok "a local image is looked for where the pivot put it; /tmp stays where it was"
else
    bad "path_after_pivot: /tmp->'$(pap /tmp/x)' /mnt/mmc->'$(pap /mnt/mmcblk0p1/x)' /root->'$(pap /root/x)'"
fi
case "$(pap rootfs.squashfs "$SB/rel")" in
    /mnt/*/rel/rootfs.squashfs | /tmp/*/rel/rootfs.squashfs)
        ok "a relative image path is made absolute before enter_ramfs leaves its cwd" ;;
    *)  bad "relative path after the pivot: '$(pap rootfs.squashfs "$SB/rel")'" ;;
esac
# The first phase keeps flashing from the path it was given: only the RAM phase
# adopts the rewritten one, so a pivot that fails still finds its images.
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -qE '^\s*(kernel|rootfs)_file=' \
    && bad "enter_ramfs must not rewrite kernel_file/rootfs_file; the in-place fallback still needs them" \
    || ok "a failed pivot falls back with the image paths it was given"
grep -q 'kernel_file=\$ramfs_kernel_file' "$SRC" && grep -q 'rootfs_file=\$ramfs_rootfs_file' "$SRC" \
    && ok "the RAM phase flashes from the paths as the pivot left them" \
    || bad "the RAM phase must adopt ramfs_kernel_file/ramfs_rootfs_file"

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

# --- RAM left once the flash phase is staged ---------------------------------
#
# A gk7201v200 upgraded through the WebUI with ~500 KB left at the pivot hung
# at rootfs erase block 17 of 77: what was still running from the old root
# faulted on the squashfs being erased, and nothing rebooted it. Short of the
# floor the run is refused before anything moves -- not handed to the in-place
# fallback, which would flash with even less in RAM.
# rmdir stubbed for the reason the test above gives.
stub rmdir 'echo "rmdir $*" >> "$FLASH_LOG"; exit 0'
reset_env
set_meminfo 1500
run -z --kernel="$K" --rootfs="$R"
rm -f "$SB/bin/rmdir"
if [ "$RC" -ne 0 ] && nothing_wrote && ! rebooted &&
    ! grep -q "pivot_root" "$SB/tmp/flash.log" &&
    printf '%s' "$OUT" | grep -q "Not enough memory left to flash safely: 1500 KB"; then
    ok "too little RAM left at the pivot refuses before anything is written"
else
    bad "short RAM at the pivot -> expected a refusal, rc=$RC out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi
if grep -q "^rmdir .*/ram$" "$SB/tmp/flash.log"; then
    ok "...and takes the staged RAM root back down"
else
    bad "a refused pivot left its RAM root behind, log='$(cat "$SB/tmp/flash.log")'"
fi

# The floor itself is enough.
reset_env
set_meminfo 2048
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && rebooted &&
    printf '%s' "$OUT" | grep -q "RAM left to flash from: 2048 KB"; then
    ok "exactly the floor is let through, and the figure is logged"
else
    bad "RAM at the floor -> expected a flash, rc=$RC out='$OUT'"
fi

# Short at the pivot too, and majestic is what gives it back: the floor is
# asked again with majestic stopped, and the run flashes.
reset_env
set_meminfo 1500
STUB_MAJESTIC_PID=901 STUB_MEM_AFTER_STOP=4000
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && rebooted &&
    grep -q "S95majestic stop" "$SB/tmp/flash.log" &&
    printf '%s' "$OUT" | grep -q "RAM left to flash from: 4000 KB with majestic stopped"; then
    ok "short at the pivot -> majestic stopped, and the flash goes on"
else
    bad "short at the pivot with majestic running -> expected a flash, rc=$RC out='$OUT' log='$(cat "$SB/tmp/flash.log")'"
fi

# --- NOR: a rootfs write hands PID 1 off too (#2647) ------------------------
#
# A squashfs on NOR can be rewritten while it is mounted, but busybox init keeps
# running from it through the erase. free_resources has dropped the page cache,
# so the next time init runs -- to reap one of the old root's processes dying
# of SIGBUS -- it faults its own text in from flash being wiped, and the
# kernel's "Attempted to kill init!" reboots with the rootfs half written. On an
# XM510 that was three runs of four.
nor_handed_off() { grep -q "^kill -QUIT 1" "$SB/tmp/flash.log"; }
reset_env
STUB_PIVOT_RC=0
run -z --kernel="$K" --rootfs="$R"
envf="$SB/ram/sysupgrade.env"
if nor_handed_off && nothing_wrote && [ -x "$SB/ram/sbin/init" ] &&
    grep -q "_handoff=1" "$envf" 2>/dev/null && ! grep -q "ubi_layout=.[a-z]" "$envf"; then
    ok "NOR: a rootfs write hands PID 1 off before anything is written (#2647)"
else
    bad "NOR rootfs write -> expected the hand-off, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# An image whose inittab has no ::restart: cannot hand off. That is no reason to
# refuse on NOR -- it is written as it always was, from a plain RAM root.
reset_env
STUB_PIVOT_RC=0
printf '::sysinit:/etc/init.d/rcS\n::shutdown:/bin/umount -a -f\n' > "$SB/etc/inittab"
run -z --kernel="$K" --rootfs="$R"
if ! nor_handed_off && grep -q "pivot_root" "$SB/tmp/flash.log" && [ ! -e "$SB/ram/sbin/init" ]; then
    ok "NOR without ::restart: still moves into a plain RAM root, not refused"
else
    bad "NOR without ::restart: -> expected the plain pivot, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# Neither does an image the hand-off would take with the old root: an SD card
# goes behind /mnt and init's exec would leave it there.
reset_env
STUB_PIVOT_RC=0
OUTSIDE="$SB/sdcard"; mkdir -p "$OUTSIDE"; cp "$R" "$OUTSIDE/"
run -z --rootfs="$OUTSIDE/$(basename "$R")"
if ! nor_handed_off && grep -q "pivot_root" "$SB/tmp/flash.log"; then
    ok "NOR with the image outside /tmp is not handed off, and still pivots"
else
    bad "NOR image outside /tmp -> expected the plain pivot, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
rm -rf "$OUTSIDE"

# A kernel-only write leaves the running rootfs alone and stays in place.
reset_env
STUB_PIVOT_RC=0
run -z --kernel="$K"
if ! nor_handed_off && ! grep -q "pivot_root" "$SB/tmp/flash.log" && flashed /dev/mtd2 && rebooted; then
    ok "NOR kernel-only: no hand-off, no pivot"
else
    bad "NOR kernel-only -> expected an in-place write, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# The second phase, as PID 1 on NOR: the old root goes before the first write.
reset_env
RUN_ENV="_ramfs_phase=1 _handoff=1 kernel_device=/dev/mtd2 update_kernel=1 update_rootfs=1 kernel_file=$K rootfs_file=$R model=ssc338q skip_soc=1 skip_ver=1 root_on_flash=1 ram_root_shipped=1"
run
u=$(logged_at "umount -l /mnt"); k=$(logged_at "flashcp .*/dev/mtd2"); r=$(logged_at "flashcp .*/dev/mtd3")
if [ -n "$u" ] && [ -n "$k" ] && [ -n "$r" ] && [ "$u" -lt "$k" ] && [ "$k" -lt "$r" ] && rebooted; then
    ok "stage 2 (NOR): old root released, then kernel and rootfs written"
else
    bad "NOR stage 2 -> expected umount -l /mnt before both writes, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

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

# --- vigil, the NVR application, gives the card up and comes back -----------
# vigil records to the card check_sdcard unmounts, and has no prepare endpoint
# for free_resources to call, so it is stopped there. A run that refuses before
# writing has to start it again -- and only once the lock is gone, or its
# supervisor (S95vigil, like S95majestic) declines the start and the NVR stays
# blank. A run that flashes leaves it to the reboot.
cat > "$SB/etc/init.d/S95vigil" <<'STUB'
#!/bin/sh
if [ "$1" != stop ] && [ -f "$STUB_LOCK" ]; then
    echo "S95vigil $1 (under the upgrade lock)" >> "$FLASH_LOG"
else
    echo "S95vigil $1" >> "$FLASH_LOG"
fi
[ "$1" = stop ] && [ -n "$STUB_VIGIL_STOP_FAILS" ] && exit 1
exit 0
STUB
chmod +x "$SB/etc/init.d/S95vigil"

reset_env
STUB_VIGILD_PID=4242
: > "$SD/autoupdate-rootfs.img"
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && grep -qx "S95vigil stop" "$SB/tmp/flash.log" &&
    grep -qx "S95vigil start" "$SB/tmp/flash.log" &&
    [ "$(logged_at "S95vigil stop")" -lt "$(logged_at "S95vigil start")" ]; then
    ok "a refused upgrade stops vigil for the card, then starts it again"
else
    bad "refused upgrade with vigil -> log='$(grep S95vigil "$SB/tmp/flash.log" | tr '\n' ';')'"
fi
if ! grep -q "S95vigil start (under the upgrade lock)" "$SB/tmp/flash.log"; then
    ok "...after the lock is gone, so its supervisor starts it"
else
    bad "vigil was started under the upgrade lock; S95vigil declines that and the NVR stays blank"
fi

# Not running when the upgrade began -- stopped on purpose, or given up on
# after a crash loop: left alone, and not started by a refused upgrade.
reset_env
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && ! grep -q "S95vigil" "$SB/tmp/flash.log"; then
    ok "a vigil that was not running is neither stopped nor started by a refused upgrade"
else
    bad "vigil not running -> log='$(grep S95vigil "$SB/tmp/flash.log" | tr '\n' ';')'"
fi

# vigil's supervisor between two vigilds -- in a crash back-off, or starting
# it again for a card that turned up -- has no vigild to find, but it is
# vigil running all the same: left alone it would see the lock and give up.
reset_env
sleep 60 &
supervisor_pid=$!
echo "$supervisor_pid" > "$SB/tmp/vigil-supervise.pid"
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
kill "$supervisor_pid" 2>/dev/null
rm -f "$SB/tmp/vigil-supervise.pid"
if [ "$RC" -ne 0 ] && grep -qx "S95vigil stop" "$SB/tmp/flash.log" &&
    grep -qx "S95vigil start" "$SB/tmp/flash.log"; then
    ok "a supervisor with no vigild at that moment is stopped and started again too"
else
    bad "supervisor only -> rc=$RC log='$(grep S95vigil "$SB/tmp/flash.log" | tr '\n' ';')'"
fi

# A stop that fails has ended the supervisor already: a refused upgrade
# restarts it rather than starting a vigild that never went away.
reset_env
STUB_VIGILD_PID=4242 STUB_VIGIL_STOP_FAILS=1
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && grep -qx "S95vigil restart" "$SB/tmp/flash.log"; then
    ok "a vigil that would not stop is restarted when the upgrade is refused"
else
    bad "failed vigil stop -> rc=$RC log='$(grep S95vigil "$SB/tmp/flash.log" | tr '\n' ';')'"
fi
rm -f "$SD/autoupdate-rootfs.img"

reset_env
STUB_VIGILD_PID=4242
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && grep -qx "S95vigil stop" "$SB/tmp/flash.log" &&
    ! grep -q "S95vigil start" "$SB/tmp/flash.log"; then
    ok "an upgrade that flashes stops vigil and leaves its start to the reboot"
else
    bad "flashing upgrade with vigil -> rc=$RC log='$(grep S95vigil "$SB/tmp/flash.log" | tr '\n' ';')'"
fi
rm -f "$SB/etc/init.d/S95vigil"

# --- majestic frees its video over the API (prepare / abort) ----------------
# The order is: majestic stops video (prepare), the card is unmounted and the
# image fetched into the RAM that freed, then flash. A majestic that answers
# prepare is not sent SIGQUIT on top of it.
reset_env
STUB_PREPARE=200
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && grep -q "majestic_api prepare" "$SB/tmp/flash.log" \
    && ! grep -q "killall" "$SB/tmp/flash.log" && flashed /dev/mtd3; then
    ok "a prepared majestic is not sent SIGQUIT, and the upgrade proceeds"
else
    bad "prepare 200 -> expected no killall and a flash, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# An older majestic has no endpoint: it gets the SIGQUIT it always got.
reset_env
STUB_PREPARE=404
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && grep -q "killall -q -3 majestic" "$SB/tmp/flash.log"; then
    ok "a majestic without the endpoint falls back to SIGQUIT"
else
    bad "prepare 404 -> expected the SIGQUIT fallback, log='$(cat "$SB/tmp/flash.log")'"
fi

# A run that gives up before writing asks a prepared majestic to put its video
# back; nothing else would -- there is no /ws/upgrade watcher on this path.
# The card it took is mounted again, exactly as /proc/mounts had it.
reset_env
STUB_PREPARE=200 STUB_ABORT=200
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
printf '/dev/mmcblk0p1 %s vfat rw,relatime 0 0\n' "$SD" >> "$SB/proc/mounts"
STUB_IMG_SOC=gk7205v300
run -z --rootfs="$R"
unset STUB_IMG_SOC
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "Remounted /dev/mmcblk0p1 on $SD"; then
    ok "a pre-write abort mounts the card it took again"
else
    bad "abort -> expected the card remounted, out='$(printf '%s' "$OUT" | tail -4)'"
fi
reset_env
STUB_PREPARE=200 STUB_ABORT=200
: > "$SD/autoupdate-rootfs.img"
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
rm -f "$SD/autoupdate-rootfs.img"
if [ "$RC" -ne 0 ] && nothing_wrote && grep -q "majestic_api abort" "$SB/tmp/flash.log" \
    && ! grep -q "S95majestic restart" "$SB/tmp/flash.log"; then
    ok "a pre-write abort asks the prepared majestic to restart its video"
else
    bad "abort after prepare -> expected majestic_api abort and no restart, rc=$RC log='$(cat "$SB/tmp/flash.log")'"
fi

# ...and if majestic cannot, it is restarted.
reset_env
STUB_PREPARE=200 STUB_ABORT=500
: > "$SD/autoupdate-rootfs.img"
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
rm -f "$SD/autoupdate-rootfs.img"
if grep -q "S95majestic restart" "$SB/tmp/flash.log"; then
    ok "...and restarts majestic when the abort fails"
else
    bad "a failed abort left majestic without video, log='$(cat "$SB/tmp/flash.log")'"
fi

# --- an archive for another camera is refused before the pivot -------------
# It unpacked fine and held nothing named for this model, and the missing file
# was only noticed inside the pivot, where die() reboots: a camera restarted for
# an upgrade that never wrote anything. Seen on an hi3516av300 given an ev300
# archive. Now it is refused while die() can still put the camera back.
reset_env
mkdir -p "$SB/other"
cp "$SB/tmp/uImage.ssc338q" "$SB/other/uImage.gk7205v300"
cp "$SB/tmp/rootfs.squashfs.ssc338q" "$SB/other/rootfs.squashfs.gk7205v300"
make_archive "$SB/other/uImage.gk7205v300" "$SB/other/rootfs.squashfs.gk7205v300"
rm -f "$SB"/tmp/*.ssc338q
STUB_PREPARE=200 STUB_ABORT=200
run -z --archive="$SB/tmp/fw.tgz"
rm -rf "$SB/other"
if [ "$RC" -ne 0 ] && nothing_wrote && ! rebooted \
    && printf '%s' "$OUT" | grep -q "Is the firmware built for" \
    && grep -q "majestic_api abort" "$SB/tmp/flash.log"; then
    ok "an archive for another camera is refused before the pivot, without a reboot"
else
    bad "wrong-model archive -> expected a pre-pivot refusal, rc=$RC rebooted=$(rebooted && echo yes || echo no) out='$(printf '%s' "$OUT" | tail -3)'"
fi

# --- a busy card does not hang the run -------------------------------------
# The unmount was never checked, so a card majestic was still recording to
# spun check_sdcard forever, holding the lock. Now it gives up, says who has the
# card, and leaves through die() with nothing written.
reset_env
STUB_UMOUNT_BUSY=1
printf '/dev/mmcblk0p1 on %s type vfat (rw,relatime)\n' "$SD" > "$SDMOUNTS"
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -q "Cannot unmount" \
    && [ ! -f "$SB/tmp/sysupgrade.lock" ]; then
    ok "a card that will not unmount aborts cleanly instead of hanging"
else
    bad "busy card -> expected a bounded abort, rc=$RC out='$(printf '%s' "$OUT" | tail -3)'"
fi

# --- a stale lock is taken over, a live one is not --------------------------
# A killed run (an ssh session dropped while it waited) used to strand the lock
# until a reboot. The lock now names its owner.
reset_env
sh -c 'exit 0' & dead=$!; wait $dead
echo "$dead" > "$SB/tmp/sysupgrade.lock"
OUT=$(cd "$SB" && env PATH="$SB/bin:$PATH" HASERLVER=1 \
    FLASH_LOG="$SB/tmp/flash.log" abort_wait=0 SDMOUNTS="$SDMOUNTS" \
    sh "$SB/sysupgrade" -z --kernel="$K" --rootfs="$R" 2>&1)
RC=$?
if [ "$RC" -eq 0 ] && printf '%s' "$OUT" | grep -q "Taking over a stale lock"; then
    ok "a lock whose owner is gone is taken over"
else
    bad "stale lock -> expected a takeover, rc=$RC out='$(printf '%s' "$OUT" | tail -3)'"
fi
rm -f "$SB/tmp/sysupgrade.lock"

reset_env
echo "$$" > "$SB/tmp/sysupgrade.lock"
OUT=$(cd "$SB" && env PATH="$SB/bin:$PATH" HASERLVER=1 \
    FLASH_LOG="$SB/tmp/flash.log" abort_wait=0 \
    sh "$SB/sysupgrade" -z --kernel="$K" --rootfs="$R" 2>&1)
RC=$?
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "already running"; then
    ok "a lock whose owner is alive is respected"
else
    bad "live lock -> expected a refusal, rc=$RC"
fi
rm -f "$SB/tmp/sysupgrade.lock"

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
echo "=== Part 1b: UBI NAND layouts ==="

# Two layouts keep kernel and rootfs in UBI volumes, told apart by root=:
#   ubifs     kernel = FIT, rootfs = UBIFS, root=ubi0:rootfs
#   ubiblock  kernel = uImage, rootfs = squashfs through ubiblock
# ubiupdatevol takes its volume exclusively, and the rootfs volume is held
# open on both (UBIFS, or ubiblock's reader), so a rootfs write on either
# goes through the PID 1 hand-off; the kernel volume is written in place.
CMDLINE_UBIFS='mem=32M console=ttyAMA0,115200 panic=20 init=/init root=ubi0:rootfs rootfstype=ubifs ubi.mtd=2,2048 mtdparts=nand:768k(boot),256k(env),-(ubi)'
CMDLINE_UBIBLOCK='mem=32M console=ttyAMA0,115200 panic=20 init=/init root=/dev/ubiblock0_1 ubi.mtd=2,2048 ubi.block=0,1 mtdparts=nand:768k(boot),256k(env),-(ubi)'
UFIT="$SB/tmp/fitImage.gk7205v500"
UFS="$SB/tmp/rootfs.ubifs.gk7205v500"
UK="$SB/tmp/uImage.gk7205v500"
US="$SB/tmp/rootfs.squashfs.gk7205v500"

# ubi_setup <ubifs|ubiblock> [rootfs_reserved_ebs]: a gk7205v500 NAND camera
# whose MTD table has no kernel/rootfs partitions -- they are UBI volumes.
ubi_setup() {
    case "$1" in
        ubifs) set_ubi "${2:-2}" "rootfs rootfs_data" 600 ;;
        *)     set_ubi "${2:-2}" ;;
    esac
    set_mtd <<'EOF2'
dev:    size   erasesize  name
mtd0: 000c0000 00020000 "boot"
mtd1: 00040000 00020000 "env"
mtd2: 07f00000 00020000 "ubi"
EOF2
    case "$1" in
        ubifs|ubifs-kvol) set_cmdline "$CMDLINE_UBIFS" ;;
        *) set_cmdline "$CMDLINE_UBIBLOCK" ;;
    esac
    # ubiblock is what SigmaStar and Rockchip NAND images still are; on the
    # SoCs that moved to the kernel-in-rootfs layout it is retired, so its
    # write path is exercised on one that did not (ssc338q). UBLOCK_SOC picks
    # a moved one instead, for the refusal.
    local soc=gk7205v500 vendor=goke
    [ "$1" = ubiblock ] && { soc=${UBLOCK_SOC:-ssc338q}; [ "$soc" = ssc338q ] && vendor=sigmastar; }
    UK="$SB/tmp/uImage.$soc"; US="$SB/tmp/rootfs.squashfs.$soc"
    set_platform ${soc}_ultimate ultimate
    export STUB_VENDOR=$vendor STUB_SOC=$soc STUB_IMG_SOC=$soc
    make_fit "$UFIT"
    make_ubifs "$UFS"
    make_uimage "$UK" $soc
    make_squashfs "$US" 8192
}
ubi_wrote() { grep -q "ubiupdatevol .*$1" "$SB/tmp/flash.log"; }
nothing_ubi() { ! grep -qE "ubiupdatevol|flashcp|flash_eraseall" "$SB/tmp/flash.log"; }
handed_off() { grep -q "^kill -QUIT 1" "$SB/tmp/flash.log"; }

# --- ubiblock: NOR artifacts; the rootfs goes through the hand-off too
# (measured: ubiupdatevol on a volume ubiblock has open is EBUSY)
reset_env; ubi_setup ubiblock
STUB_PIVOT_RC=0
run -z --kernel="$UK" --rootfs="$US"
if handed_off && nothing_ubi && grep -q "ubi_layout=.ubiblock" "$SB/ram/sysupgrade.env"; then
    ok "ubiblock: a rootfs write hands PID 1 off before anything is written"
else
    bad "ubiblock: expected the hand-off, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
reset_env; ubi_setup ubiblock
run -z --kernel="$UK"
if [ "$RC" -eq 0 ] && ubi_wrote "/dev/ubi0_0 $UK" && ! handed_off && ! grep -q flashcp "$SB/tmp/flash.log"; then
    ok "ubiblock: a uImage is written into the kernel volume in place"
else
    bad "ubiblock: kernel-only should write ubi0_0 in place, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
reset_env; ubi_setup ubiblock
RUN_ENV="_ramfs_phase=1 _handoff=1 ubi_layout=ubiblock kernel_device=/dev/ubi0_0 ubi_rootfs_dev=/dev/ubi0_1 ubi_data_dev=/dev/ubi0_2 update_kernel=1 update_rootfs=1 kernel_file=$UK rootfs_file=$US model=ssc338q skip_soc=1 skip_ver=1 root_on_flash=1 ram_root_shipped=1"
run
u=$(logged_at "umount -l /mnt"); k=$(logged_at "ubiupdatevol /dev/ubi0_0 $UK"); r=$(logged_at "ubiupdatevol /dev/ubi0_1 $US")
if [ -n "$u" ] && [ -n "$k" ] && [ -n "$r" ] && [ "$u" -lt "$k" ] && [ "$k" -lt "$r" ] && rebooted; then
    ok "stage 2 (ubiblock): old root released, uImage then squashfs written"
else
    bad "stage 2 ubiblock order ${u:-none}/${k:-none}/${r:-none} log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# A hand-off lets go of the old root -- an SD card behind it included -- before
# the first write, so an image there would be gone when the flash looks for it.
# Only a path outside /tmp shows that, and the sandbox lives under /tmp.
OUTSIDE=$(mktemp -d "${HOME:-/root}/.sysupgrade-test.XXXXXX" 2>/dev/null)
case "$(realpath "${OUTSIDE:-/tmp}" 2>/dev/null)" in
    /tmp/* | /tmp | "")
        ok "hand-off with an image outside /tmp: skipped, no directory outside /tmp to stage one in" ;;
    *)
        reset_env; ubi_setup ubiblock
        cp "$US" "$OUTSIDE/"
        STUB_PIVOT_RC=0
        run -z --kernel="$UK" --rootfs="$OUTSIDE/$(basename "$US")"
        if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "copy it into /tmp" &&
            nothing_ubi && ! handed_off && ! grep -q pivot_root "$SB/tmp/flash.log"; then
            ok "hand-off: an image the old root takes with it is refused before anything moves (#2605)"
        else
            bad "hand-off with an image outside /tmp must be refused, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
        fi
        # ...but only an image the run will write. On ubifs a --kernel beside
        # --rootfs is dropped (the kernel is inside the rootfs), so where that
        # one lives cannot matter.
        reset_env; ubi_setup ubifs
        make_fit_soc "$UFIT" gk7205v500
        cp "$UFIT" "$OUTSIDE/"
        STUB_PIVOT_RC=0
        run -z --force_soc --kernel="$OUTSIDE/$(basename "$UFIT")" --rootfs="$UFS"
        if handed_off && ! printf '%s' "$OUT" | grep -q "copy it into /tmp"; then
            ok "hand-off: a --kernel the ubifs layout drops is not held against the run"
        else
            bad "ubifs: an unused --kernel outside /tmp must not refuse the run, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
        fi ;;
esac
[ -n "$OUTSIDE" ] && rm -rf "$OUTSIDE"

# --- ubifs: the kernel is a file in the rootfs, so -k writes the rootfs
reset_env; ubi_setup ubifs
make_fit_soc "$UFIT" gk7205v500
STUB_PIVOT_RC=0
run -z --kernel="$UFIT" --rootfs="$UFS"
if printf '%s' "$OUT" | grep -q "kernel lives inside the rootfs" && handed_off && nothing_ubi; then
    ok "ubifs: -k means writing the rootfs, through the hand-off"
else
    bad "ubifs: -k should become a rootfs write, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- the retired layout with a kernel volume of its own is reinstalled, not upgraded
reset_env; ubi_setup ubifs-kvol
STUB_PIVOT_RC=0
run -z --kernel="$UFIT" --rootfs="$UFS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "retired NAND layout with a separate kernel volume" &&
    printf '%s' "$OUT" | grep -q "ipctool nand-migrate" &&
    nothing_ubi && ! handed_off && ! rebooted; then
    ok "ubifs-kvol: refused before anything is touched, nand-migrate and reinstall link given"
else
    bad "ubifs-kvol must be refused, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- ubifs: a local UBIFS rootfs has no SoC witness
reset_env; ubi_setup ubifs
run -z --kernel="$UFIT" --rootfs="$UFS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "Cannot verify the SoC of a UBIFS rootfs" && nothing_ubi; then
    ok "ubifs: a local UBIFS rootfs is refused without --force_soc, nothing written"
else
    bad "ubifs: expected the SoC refusal before any write, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- ubifs: no ::restart: entry -> refused before the kernel goes down
reset_env; ubi_setup ubifs
printf '::sysinit:/etc/init.d/rcS\n' > "$SB/etc/inittab"
run -z -f --kernel="$UFIT" --rootfs="$UFS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "::restart:/sbin/init" && nothing_ubi && ! rebooted; then
    ok "ubifs: an inittab that cannot hand PID 1 off is refused, nothing written, no reboot"
else
    bad "ubifs: expected the inittab refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- ubifs: no pivot -> no in-place fallback, refused unwritten
reset_env; ubi_setup ubifs
run -z -f --kernel="$UFIT" --rootfs="$UFS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "cannot be rewritten without it" && nothing_ubi && ! rebooted; then
    ok "ubifs: a failed pivot refuses rather than writing a volume that is in use"
else
    bad "ubifs: expected the no-pivot refusal, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- format must match what root= mounts
reset_env; ubi_setup ubifs
run -z -f --rootfs="$US"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "boots a UBIFS rootfs" && nothing_ubi; then
    ok "ubifs: a squashfs rootfs is refused"
else
    bad "ubifs: a squashfs must be refused, rc=$RC out='$OUT'"
fi
reset_env; ubi_setup ubiblock
run -z -f --rootfs="$UFS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "is a UBIFS image" && nothing_ubi; then
    ok "ubiblock: a UBIFS rootfs is refused"
else
    bad "ubiblock: a UBIFS image must be refused, rc=$RC out='$OUT'"
fi

# --- a UBIFS made for other LEBs would never mount
reset_env; ubi_setup ubifs
make_ubifs "$UFS" 253952
run -z -f --kernel="$UFIT" --rootfs="$UFS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "253952-byte LEBs" && nothing_ubi; then
    ok "ubifs: an image for another LEB size is refused before the kernel is written"
else
    bad "ubifs: LEB mismatch must be refused, rc=$RC out='$OUT'"
fi

# --- bigger than the UBI device leaves room for, beside the settings
reset_env; ubi_setup ubifs 1
echo 24 > "$SB/sys/class/ubi/ubi0_1/reserved_ebs"
dd if=/dev/zero bs=1k count=200 >> "$UFS" 2>/dev/null
run -z -f --kernel="$UFIT" --rootfs="$UFS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "does not fit" && nothing_ubi; then
    ok "ubifs: a rootfs larger than its volume is refused, nothing written"
else
    bad "ubifs: oversize rootfs must be refused, rc=$RC out='$OUT'"
fi

# --- the hand-off itself
reset_env; ubi_setup ubifs
STUB_PIVOT_RC=0
run -z -f --kernel="$UFIT" --rootfs="$UFS"
envf="$SB/ram/sysupgrade.env"
if handed_off && nothing_ubi && [ -x "$SB/ram/sbin/init" ] && [ ! -e "$SB/ram/bin/umount" ] &&
    [ -L "$SB/ram/sbin/umount" ] &&
    grep -q "_handoff=1" "$envf" 2>/dev/null && grep -q "_ramfs_phase=.1" "$envf" &&
    grep -q "ubi_rootfs_dev=./dev/ubi0_0" "$envf" && grep -q "sysupgrade.env" "$SB/ram/sbin/init"; then
    ok "ubifs: the RAM root is staged for PID 1 and SIGQUIT sent before any write"
    printf '%s' "$OUT" | grep -q "SSH, the WebUI and every other login end here" &&
        ok "ubifs: the hand-off says the session ends, rather than that it goes quiet" ||
        bad "ubifs: hand-off must say the session ends, out='$OUT'"
else
    bad "ubifs: hand-off staging, log='$(cat "$SB/tmp/flash.log")' init='$(cat "$SB/ram/sbin/init" 2>&1)' out='$OUT'"
fi

# --- #2536: the unpack, and the RAM root after it, measured against memory
# they can actually have. A gk7205v500 built without CONFIG_SHMEM has ramfs
# behind every tmpfs, and ramfs pages cannot be placed in CMA: it passed this
# check with 99 MB "available", 98 MB of it free CMA, and was OOM-killed at the
# hand-off with nothing written.
REAL_STAT=$(command -v stat)
# set_meminfo_cma <MemAvailable> <CmaFree>
set_meminfo_cma() {
    printf 'MemTotal:       %8d kB\nMemFree:        %8d kB\nMemAvailable:   %8d kB\nCmaTotal:       %8d kB\nCmaFree:        %8d kB\n' \
        131072 "$1" "$1" 98304 "$2" > "$SB/proc/meminfo"
}
# What statfs says of /tmp on that kernel; every other stat call is the real one.
stub_ramfs_tmp() {
    stub stat "[ \"\$1 \$2 \$3\" = '-f -c %t' ] && { echo 858458f6; exit 0; }; exec $REAL_STAT \"\$@\""
}

reset_env; ubi_setup ubifs
make_archive "$UFIT" "$UFS"
STUB_PIVOT_RC=0
stub_ramfs_tmp
set_meminfo_cma 99000 98900
run -z -f --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && nothing_ubi && ! handed_off \
    && printf '%s' "$OUT" | grep -q "cannot use the 98900 KB free in the"; then
    ok "ramfs /tmp: free CMA is not counted, so the unpack is refused before the hand-off"
else
    bad "ramfs /tmp + CMA -> expected a refusal, rc=$RC out='$OUT'"
fi

# The same numbers with a real tmpfs, whose pages CMA does take: no refusal.
rm -f "$SB/bin/stat"
reset_env; ubi_setup ubifs
make_archive "$UFIT" "$UFS"
STUB_PIVOT_RC=0
set_meminfo_cma 99000 98900
run -z -f --archive="$SB/tmp/fw.tgz"
if handed_off && ! printf '%s' "$OUT" | grep -q "Not enough memory"; then
    ok "...a real tmpfs may use that CMA, and the run hands off"
else
    bad "tmpfs /tmp + CMA must not be refused, rc=$RC out='$OUT'"
fi

# ...and ramfs on a kernel with no CMA changes nothing: MemAvailable is right.
reset_env; ubi_setup ubifs
make_archive "$UFIT" "$UFS"
STUB_PIVOT_RC=0
stub_ramfs_tmp
set_meminfo 99000
run -z -f --archive="$SB/tmp/fw.tgz"
rm -f "$SB/bin/stat"
if handed_off && ! printf '%s' "$OUT" | grep -q "Not enough memory"; then
    ok "...nor is ramfs on a kernel without CMA"
else
    bad "ramfs /tmp without CMA must not be refused, rc=$RC out='$OUT'"
fi

# The RAM left at the pivot is counted the same way: 3000 KB "available", 2000
# of it free CMA a ramfs kernel cannot use, is 1000 KB, under the floor.
stub rmdir 'echo "rmdir $*" >> "$FLASH_LOG"; exit 0'
reset_env
stub_ramfs_tmp
set_meminfo_cma 3000 2000
run -z --kernel="$K" --rootfs="$R"
rm -f "$SB/bin/stat" "$SB/bin/rmdir"
if [ "$RC" -ne 0 ] && nothing_wrote && ! rebooted &&
    printf '%s' "$OUT" | grep -q "RAM left to flash from: 1000 KB (free CMA not counted: 2000 KB)" &&
    printf '%s' "$OUT" | grep -q "Not enough memory left to flash safely: 1000 KB"; then
    ok "ramfs /tmp: free CMA is not counted at the pivot either"
else
    bad "ramfs /tmp + CMA at the pivot -> expected a refusal, rc=$RC out='$OUT'"
fi

# ...and a real tmpfs with the same numbers has its 3000 KB.
reset_env
set_meminfo_cma 3000 2000
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 &&
    printf '%s' "$OUT" | grep -q "RAM left to flash from: 3000 KB$"; then
    ok "...while a real tmpfs counts that CMA, and flashes"
else
    bad "tmpfs /tmp + CMA at the pivot must not be refused, rc=$RC out='$OUT'"
fi

# A run that must hand PID 1 over cannot fall back to flashing in place, so the
# RAM root it stages after the unpack is reserved too. Learn the two figures
# from a refusal, then sit between "image + reserve" and "+ the RAM root".
reset_env; ubi_setup ubifs
make_archive "$UFIT" "$UFS"
set_meminfo 1
run -z -f --archive="$SB/tmp/fw.tgz"
n=$(printf '%s' "$OUT" | sed -n 's/.*KB -- \([0-9]*\) KB of image.*/\1/p' | head -1)
s=$(printf '%s' "$OUT" | sed -n 's/.* and \([0-9]*\) KB for the RAM root.*/\1/p' | head -1)
if [ -n "$n" ] && [ -n "$s" ] && [ "$s" -gt 1 ]; then
    reset_env; ubi_setup ubifs
    make_archive "$UFIT" "$UFS"
    STUB_PIVOT_RC=0
    set_meminfo $((n + 512 + s / 2))
    run -z -f --archive="$SB/tmp/fw.tgz"
    if [ "$RC" -ne 0 ] && nothing_ubi && ! handed_off \
        && printf '%s' "$OUT" | grep -q "KB for the RAM root"; then
        ok "a hand-off run reserves the RAM root it stages after the unpack"
    else
        bad "image+reserve fits but the RAM root does not -> expected a refusal, rc=$RC out='$OUT'"
    fi
else
    bad "could not read the image and RAM-root figures from the refusal, n='$n' s='$s' out='$OUT'"
fi
set_meminfo

# ...and counted the way ramfs takes it: a page per applet link, which a real
# tmpfs keeps inline. 250 links are 1000 KB on ramfs and nothing on tmpfs.
ram_root_kb() {
    reset_env; ubi_setup ubifs
    make_archive "$UFIT" "$UFS"
    set_meminfo 1
    RUN_ENV="STUB_BB_APPLETS=250" run -z -f --archive="$SB/tmp/fw.tgz"
    printf '%s' "$OUT" | sed -n 's/.* and \([0-9]*\) KB for the RAM root.*/\1/p' | head -1
}
on_tmpfs=$(ram_root_kb)
stub_ramfs_tmp
on_ramfs=$(ram_root_kb)
rm -f "$SB/bin/stat"
set_meminfo
if [ -n "$on_tmpfs" ] && [ -n "$on_ramfs" ] && [ "$((on_ramfs - on_tmpfs))" -eq 1000 ]; then
    ok "the RAM root on ramfs reserves a page per applet link"
else
    bad "RAM root on tmpfs '$on_tmpfs' KB, on ramfs '$on_ramfs' KB: expected 1000 KB apart"
fi

# --- stage 2, as PID 1: release the old root, copy the settings out, rebuild
# the volumes around the new image, put the settings back, reboot
S2="_ramfs_phase=1 _handoff=1 ubi_layout=ubifs kernel_device=/rom/boot/uImage ubi_rootfs_dev=/dev/ubi0_0 ubi_data_dev=/dev/ubi0_1 model=gk7205v500 skip_soc=1 skip_ver=1 root_on_flash=1 ram_root_shipped=1 overlay_kb=64"
reset_env; ubi_setup ubifs
RUN_ENV="$S2 update_rootfs=1 rootfs_file=$UFS"
run
u=$(logged_at "umount -l /mnt"); bk=$(logged_at "mount ubifs ro"); rm=$(logged_at "ubirmvol /dev/ubi0 -N rootfs_data")
rs=$(logged_at "ubirsvol /dev/ubi0 -n 0 -s $(stat -c %s "$UFS")"); w=$(logged_at "ubiupdatevol /dev/ubi0_0 $UFS")
mk=$(logged_at "ubimkvol /dev/ubi0 -N rootfs_data -m"); rs2=$(logged_at "mount ubifs rw"); b=$(logged_at "^reboot")
if [ -n "$u" ] && [ -n "$bk" ] && [ -n "$rm" ] && [ -n "$rs" ] && [ -n "$w" ] && [ -n "$mk" ] && [ -n "$rs2" ] && [ -n "$b" ] &&
    [ "$u" -lt "$bk" ] && [ "$bk" -lt "$rm" ] && [ "$rm" -lt "$rs" ] && [ "$rs" -lt "$w" ] && [ "$w" -lt "$mk" ] &&
    [ "$mk" -lt "$rs2" ] && [ "$rs2" -lt "$b" ] && [ -f "$SB/tmp/overlay.old/etc/marker" ] &&
    printf '%s' "$OUT" | grep -q "Settings carried over"; then
    ok "stage 2: settings copied, volumes resized to the image, settings restored, then reboot"
else
    bad "stage 2 order release/copy/rmvol/rsvol/write/mkvol/restore/reboot = ${u:-none}/${bk:-none}/${rm:-none}/${rs:-none}/${w:-none}/${mk:-none}/${rs2:-none}/${b:-none} log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- the rootfs write says how far it has got: busybox ubiupdatevol does not,
# and a NAND flash otherwise sat silent through the one step that matters
reset_env; ubi_setup ubifs
STUB_UBI_SLOW=1
RUN_ENV="$S2 update_rootfs=1 rootfs_file=$UFS"
run
if ubi_wrote "/dev/ubi0_0 $UFS" && rebooted &&
    printf '%s\n' "$OUT" | grep -qE "^Written: [0-9]{1,2}% \([0-9.]+ of [0-9.]+ MB\)$" &&
    printf '%s\n' "$OUT" | grep -q "^Written: 100%"; then
    ok "stage 2: the rootfs write reports its progress, then 100%"
else
    bad "stage 2 progress, log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
reset_env; ubi_setup ubifs
STUB_UBI_SLOW=1 STUB_FLASHCP_FAIL=1
RUN_ENV="$S2 update_rootfs=1 rootfs_file=$UFS"
run
if printf '%s' "$OUT" | grep -q "Writing $UFS to /dev/ubi0_0 failed" &&
    ! printf '%s' "$OUT" | grep -q "Written: 100%" && ! grep -q "ubimkvol" "$SB/tmp/flash.log"; then
    ok "stage 2: a failed write is still a failure behind the progress, and never reads 100%"
else
    bad "stage 2 failed write behind progress, log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
reset_env; ubi_setup ubifs
STUB_UBI_SLOW=1
RUN_ENV="$S2 update_rootfs=1 rootfs_file=$UFS silent_update=1"
run
if ubi_wrote "/dev/ubi0_0 $UFS" && ! printf '%s' "$OUT" | grep -q "Written:"; then
    ok "stage 2: under -s the WebUI gets set_progress's lines and nothing else"
else
    bad "stage 2 -s progress, out='$OUT'"
fi

# --- a handed-off flash phase logs to a file the console and nc both follow
reset_env; ubi_setup ubifs
FOLLOW_LOG="$SB/tmp/upgrade.log"; rm -f "$FOLLOW_LOG" "$FOLLOW_LOG.http"
RUN_ENV="$S2 update_rootfs=1 rootfs_file=$UFS"
run
if grep -q "Flashing from RAM" "$FOLLOW_LOG" && grep -q "Unconditional reboot" "$FOLLOW_LOG" &&
    grep -q "^tail -n +1 -f $FOLLOW_LOG" "$SB/tmp/flash.log" &&
    grep -q "^nc -ll -p 80 -e $FOLLOW_LOG.http" "$SB/tmp/flash.log" && [ -x "$FOLLOW_LOG.http" ] &&
    head -1 "$FOLLOW_LOG.http" | grep -qx '#!/bin/sh' && grep -q 'HTTP/1.0 200 OK' "$FOLLOW_LOG.http" &&
    ubi_wrote "/dev/ubi0_0 $UFS" && rebooted; then
    ok "stage 2: the log goes to a file, the console tails it and nc serves it"
else
    bad "stage 2 follow log, log='$(cat "$SB/tmp/flash.log")' follow='$(cat "$FOLLOW_LOG" 2>&1)' out='$OUT'"
fi
reset_env; ubi_setup ubifs
RUN_ENV="$S2 update_rootfs=1 rootfs_file=$UFS"
run
if printf '%s' "$OUT" | grep -q "Flashing from RAM" && ! grep -qE "^(tail|nc) " "$SB/tmp/flash.log"; then
    ok "stage 2: an empty FOLLOW_LOG leaves the output where it was"
else
    bad "stage 2 FOLLOW_LOG off, log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- -r with -n: the new overlay is the wipe; nothing is copied or restored
reset_env; ubi_setup ubifs
RUN_ENV="$S2 update_rootfs=1 rootfs_file=$UFS clear_overlay=1"
run
if ubi_wrote "/dev/ubi0_0 $UFS" && grep -q "ubimkvol /dev/ubi0 -N rootfs_data -m" "$SB/tmp/flash.log" &&
    ! grep -q "mount ubifs" "$SB/tmp/flash.log" && ! grep -q "ubiupdatevol -t" "$SB/tmp/flash.log" && rebooted; then
    ok "stage 2: -r -n rebuilds an empty settings volume and copies nothing"
else
    bad "stage 2 -r -n, log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

reset_env; ubi_setup ubifs
RUN_ENV="$S2 clear_overlay=1"
run
if grep -q "ubiupdatevol -t /dev/ubi0_1" "$SB/tmp/flash.log" && ! grep -q "ubirmvol" "$SB/tmp/flash.log" && rebooted; then
    ok "stage 2: -n alone truncates the UBIFS overlay volume"
else
    bad "stage 2 wipe: expected ubiupdatevol -t /dev/ubi0_1, log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- -n on a mounted UBIFS overlay needs the hand-off too
reset_env; ubi_setup ubifs; set_mounts ubi
run -z -n
if [ "$RC" -ne 0 ] && nothing_ubi && ! rebooted; then
    ok "ubifs: wiping a mounted UBIFS overlay without a pivot is refused, nothing erased"
else
    bad "ubifs: -n without a pivot must refuse, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- the FIT beside a UBIFS rootfs is its SoC witness
reset_env; ubi_setup ubifs
make_fit_soc "$UFIT" gk7205v500
run -z --kernel="$UFIT" --rootfs="$UFS"
if printf '%s' "$OUT" | grep -q "SoC from the FIT kernel beside it: gk7205v500" &&
    printf '%s' "$OUT" | grep -q "SoC OK" && nothing_ubi; then
    ok "ubifs: a FIT naming this SoC vouches for the UBIFS rootfs beside it"
else
    bad "ubifs: FIT witness, rc=$RC out='$OUT'"
fi
reset_env; ubi_setup ubifs
make_fit_soc "$UFIT" hi3516ev300
run -z --kernel="$UFIT" --rootfs="$UFS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "Wrong SoC" && nothing_ubi && ! rebooted; then
    ok "ubifs: a FIT for another SoC stops the run before anything is written"
else
    bad "ubifs: foreign FIT must be refused, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- a local archive is not pinned by its names
reset_env; ubi_setup ubifs
stage="$SB/stage"; rm -rf "$stage"; mkdir -p "$stage"
cp "$UFIT" "$UFS" "$stage/"
(cd "$stage" && for f in fitImage.gk7205v500 rootfs.ubifs.gk7205v500; do
    md5sum "$f" > "$f.md5sum"; done && tar cf - . | gzip > "$SB/tmp/fw.tgz")
rm -f "$UFIT" "$UFS"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "no FIT kernel beside it names one" && nothing_ubi; then
    ok "ubifs: an archive whose FIT names no SoC is not taken on its file names"
else
    bad "ubifs: archive without a witness must be refused, rc=$RC out='$OUT'"
fi

# --- stage 2 writes nothing when the old root will not let go
reset_env; ubi_setup ubifs
RUN_ENV="STUB_BB_UMOUNT_RC=1 $S2 update_rootfs=1 rootfs_file=$UFS"
run
if printf '%s' "$OUT" | grep -q "Could not let go of the old root" && nothing_ubi &&
    ! grep -q "ubirmvol" "$SB/tmp/flash.log" && rebooted; then
    ok "stage 2: a failed release of the old root reboots with nothing written"
else
    bad "stage 2: umount failure must stop before the first write, log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- a ubiblock camera from before the hand-off keeps its gluebi path
reset_env; ubi_setup ubiblock
printf '::sysinit:/etc/init.d/rcS\n' > "$SB/etc/inittab"
set_mtd <<'EOF2'
dev:    size   erasesize  name
mtd0: 000c0000 00020000 "boot"
mtd1: 00040000 00020000 "env"
mtd2: 07f00000 00020000 "ubi"
mtd3: 003e0000 0001f000 "kernel"
mtd4: 01f00000 0001f000 "rootfs"
mtd5: 00200000 0001f000 "rootfs_data"
EOF2
run -z --kernel="$UK" --rootfs="$US"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && flashed /dev/mtd4 && ! handed_off &&
    ! grep -q ubiupdatevol "$SB/tmp/flash.log"; then
    ok "ubiblock without ::restart: writes through gluebi as it always did"
else
    bad "ubiblock old-inittab fallback, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
reset_env; ubi_setup ubiblock
printf '::sysinit:/etc/init.d/rcS\n' > "$SB/etc/inittab"
run -z --kernel="$UK" --rootfs="$US"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "::restart:/sbin/init" && nothing_ubi && ! rebooted; then
    ok "ubiblock without ::restart: and without gluebi is refused, nothing written"
else
    bad "ubiblock old inittab, no gluebi, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- the retired HiSilicon split NAND layout: uImage in a raw `kernel`
# partition, UBIFS root in a UBI device beside it, no `kernel` volume. gluebi
# names the UBIFS volume "rootfs" in /proc/mtd, which is what the MTD path
# would have flashed a NOR squashfs over.
split_setup() {
    local u="$SB/sys/class/ubi" i=0 name
    rm -rf "$u"
    for name in rootfs rootfs_data; do
        mkdir -p "$u/ubi0_$i"
        echo "$name" > "$u/ubi0_$i/name"
        echo 126976 > "$u/ubi0_$i/usable_eb_size"
        echo 260 > "$u/ubi0_$i/reserved_ebs"
        i=$((i + 1))
    done
    set_mtd <<'EOF2'
dev:    size   erasesize  name
mtd0: 00100000 00020000 "boot"
mtd1: 00100000 00020000 "env"
mtd2: 00800000 00020000 "kernel"
mtd3: 07600000 00020000 "ubi"
mtd4: 02017000 0001f000 "rootfs"
mtd5: 04f51000 0001f000 "rootfs_data"
EOF2
    set_cmdline 'mem=128M console=ttyAMA0,115200 panic=20 rootfstype=ubifs root=ubi0:rootfs ubi.mtd=3,2048 mtdparts=hinand:1024k(boot),1024k(env),8192k(kernel),-(ubi)'
    set_platform hi3516ev300_ultimate ultimate
    export STUB_VENDOR=hisilicon STUB_SOC=hi3516ev300 STUB_IMG_SOC=hi3516ev300
    SK="$SB/tmp/uImage.hi3516ev300"; SS="$SB/tmp/rootfs.squashfs.hi3516ev300"
    make_uimage "$SK" hi3516ev300
    make_squashfs "$SS" 8192
}
reset_env; split_setup
STUB_PIVOT_RC=0
run -z --kernel="$SK" --rootfs="$SS"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "retired split NAND layout" &&
    printf '%s' "$OUT" | grep -q "openipc.org/cameras/vendors/hisilicon/socs/hi3516ev300" &&
    ! grep -qE "flashcp|ubiupdatevol|flash_eraseall|pivot_root" "$SB/tmp/flash.log" && ! rebooted; then
    ok "split NAND: kernel+rootfs refused before anything is touched, reinstall link given"
else
    bad "split NAND -k -r, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
reset_env; split_setup
run -z --kernel="$SK"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "retired split NAND layout" && nothing_wrote && ! rebooted; then
    ok "split NAND: a kernel-only run is refused too"
else
    bad "split NAND -k, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
reset_env; split_setup
run -z -n
if ! printf '%s' "$OUT" | grep -q "retired split NAND layout"; then
    ok "split NAND: -n alone is not refused"
else
    bad "split NAND -n should not be refused, rc=$RC out='$OUT'"
fi

# --- an hi3516ev300 on the UBI-only layout is an ordinary ubifs camera
ubi_setup_hisi() {
    ubi_setup ubifs
    set_cmdline 'mem=32M console=ttyAMA0,115200 panic=20 init=/init root=ubi0:rootfs rootfstype=ubifs ubi.mtd=2,2048 mtdparts=hinand:768k(boot),256k(env),-(ubi)'
    set_platform hi3516ev300_ultimate ultimate
    export STUB_VENDOR=hisilicon STUB_SOC=hi3516ev300 STUB_IMG_SOC=hi3516ev300
}
reset_env; ubi_setup_hisi
HFIT="$SB/tmp/fitImage.hi3516ev300"; make_fit_soc "$HFIT" hi3516ev300
HFS="$SB/tmp/rootfs.ubifs.hi3516ev300"; make_ubifs "$HFS"
STUB_PIVOT_RC=0
run -z --kernel="$HFIT" --rootfs="$HFS"
if printf '%s' "$OUT" | grep -q "SoC from the FIT kernel beside it: hi3516ev300" && handed_off && nothing_ubi &&
    ! printf '%s' "$OUT" | grep -qE "retired (split )?NAND layout"; then
    ok "hi3516ev300 UBI-only: an ordinary ubifs camera, rootfs write handed off"
else
    bad "hi3516ev300 UBI-only, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
# --- a device profile and a local UBIFS archive ----------------------------
# The rootfs cannot be read without mounting it, so on a device profile only a
# download under the device's own name used to count, and every local archive
# needed --force_soc -- which gives up the SoC check as well. The NAND FIT now
# carries the build's BUILD_PLATFORM (openipc,platform in nand-fit.its).
# The value follows an FDT property header as it does in a real FIT: token,
# length, then the name offset, whose last byte here is printable ('A').
make_fit_plat() {
    make_fit_soc "$1" "$2"
    printf '\0\0\0\3\0\0\0\100\0\0\0\101BUILD_PLATFORM=%s\0' "$3" >> "$1"
}
# hisi_profile_archive <FIT platform, or empty>: a profile camera and a local
# archive whose FIT names that platform; the loose images are removed, so what
# /tmp holds afterwards is what the run left there.
hisi_profile_archive() {
    reset_env; ubi_setup_hisi
    set_platform hi3516ev300_ultimate_acme-cam1 ultimate
    if [ -n "$1" ]; then make_fit_plat "$HFIT" hi3516ev300 "$1"; else make_fit_soc "$HFIT" hi3516ev300; fi
    make_ubifs "$HFS"
    make_archive "$HFIT" "$HFS"
    rm -f "$HFIT" "$HFS"
    STUB_PIVOT_RC=0
}

hisi_profile_archive hi3516ev300_ultimate_acme-cam1
run -z --archive="$SB/tmp/fw.tgz"
if printf '%s' "$OUT" | grep -q "Device profile from the FIT kernel beside it: hi3516ev300_ultimate_acme-cam1" &&
    printf '%s' "$OUT" | grep -q "SoC from the FIT kernel beside it: hi3516ev300" &&
    handed_off && [ ! -f "$SB/tmp/fw.tgz" ]; then
    ok "device profile + local UBIFS archive whose FIT names the device -> proceeds, SoC still checked"
else
    bad "the device's own local archive should pass, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# Another device's image, and what the refusal leaves: the archive, to retry
# from, and none of what it unpacked -- on a camera that was 17 MB of a 60 MB
# tmpfs, and the archive itself was already gone. A file somebody else keeps
# in /tmp is not the unpack's to remove.
hisi_profile_archive hi3516ev300_ultimate_other-cam2
echo theirs > "$SB/tmp/not-from-the-archive"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "Wrong platform" && nothing_ubi && ! handed_off &&
    [ -f "$SB/tmp/fw.tgz" ] && [ ! -e "$HFIT" ] && [ ! -e "$HFS" ] &&
    [ ! -e "$SB/tmp/openipc.md5sum" ] && [ -f "$SB/tmp/not-from-the-archive" ]; then
    ok "device profile + another device's UBIFS archive -> refused; archive kept, unpacked images removed"
else
    bad "another device's archive, rc=$RC archive=$([ -f "$SB/tmp/fw.tgz" ] && echo kept || echo gone) left=$(ls "$SB/tmp" | tr '\n' ' ') out='$OUT'"
fi
rm -f "$SB/tmp/not-from-the-archive"

# The record of an unpack belongs to the run that made it. A second sysupgrade
# that fails before unpacking anything must not act on another run's list --
# here one at the name an earlier draft used, naming an image that run is about
# to flash.
reset_env
echo "fitImage.hi3516ev300" > "$SB/tmp/sysupgrade.unpacked"
echo running > "$SB/tmp/fitImage.hi3516ev300"
run -z --archive="$SB/tmp/no-such.tgz"
if [ "$RC" -ne 0 ] && [ -f "$SB/tmp/fitImage.hi3516ev300" ]; then
    ok "a run that unpacked nothing leaves another run's images alone"
else
    bad "a failed run removed what it did not unpack, rc=$RC out='$OUT'"
fi
rm -f "$SB/tmp/sysupgrade.unpacked" "$SB/tmp/fitImage.hi3516ev300"

# A FIT from before it carried a platform is no evidence either way.
hisi_profile_archive ""
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "names no platform" && nothing_ubi; then
    ok "device profile + UBIFS archive whose FIT names no platform -> refused"
else
    bad "a FIT with no platform proves nothing, rc=$RC out='$OUT'"
fi

# --force_soc still takes it, as the refusal says.
hisi_profile_archive hi3516ev300_ultimate_other-cam2
run -z --force_soc --archive="$SB/tmp/fw.tgz"
if handed_off && printf '%s' "$OUT" | grep -q "Skipping the rootfs SoC pre-check"; then
    ok "device profile + another device's UBIFS archive + --force_soc -> proceeds"
else
    bad "--force_soc should still override, rc=$RC out='$OUT'"
fi
rm -f "$HFIT" "$HFS"

default_url_is "https://github.com/OpenIPC/firmware/releases/download/latest/openipc.hi3516ev300-nand-ultimate.tgz" \
    ubi_setup_hisi

# --- the download: -nand- for ubifs, -nor- for ubiblock
F=https://github.com/OpenIPC/firmware/releases/download/latest
default_url_is "$F/openipc.gk7205v500-nand-ultimate.tgz" ubi_setup ubifs
default_url_is "$F/openipc.ssc338q-nor-ultimate.tgz" ubi_setup ubiblock

# --- on a SoC that moved to the kernel-in-rootfs layout, ubiblock is retired
reset_env; UBLOCK_SOC=gk7205v500 ubi_setup ubiblock
STUB_PIVOT_RC=0
run -z --kernel="$UK" --rootfs="$US"
# nand-migrate needs a FIT U-Boot, which a ubiblock camera does not have
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "retired NAND layout with a separate kernel volume" &&
    ! printf '%s' "$OUT" | grep -q "nand-migrate" &&
    nothing_ubi && ! handed_off && ! rebooted; then
    ok "ubiblock on gk7205v500: refused before anything is touched, reinstall link given"
else
    bad "ubiblock on a moved SoC must be refused, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi
reset_env; UBLOCK_SOC=gk7205v500 ubi_setup ubiblock
printf '::sysinit:/etc/init.d/rcS\n' > "$SB/etc/inittab"
set_mtd <<'EOF2'
dev:    size   erasesize  name
mtd0: 000c0000 00020000 "boot"
mtd1: 00040000 00020000 "env"
mtd2: 07f00000 00020000 "ubi"
mtd3: 003e0000 0001f000 "kernel"
mtd4: 01f00000 0001f000 "rootfs"
mtd5: 00200000 0001f000 "rootfs_data"
EOF2
run -z --kernel="$UK" --rootfs="$US"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "retired NAND layout" && nothing_ubi && ! rebooted; then
    ok "ubiblock on gk7205v500 without ::restart: is refused too, not written through gluebi"
else
    bad "old-inittab ubiblock on a moved SoC must be refused, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- ubifs: a local kernel file cannot be written on its own
reset_env; ubi_setup ubifs
make_fit_soc "$UFIT" gk7205v500
run -z --kernel="$UFIT"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "pass the rootfs.ubifs that carries it" && nothing_ubi && ! handed_off; then
    ok "ubifs: --kernel=FILE alone is refused, not swapped for some other rootfs"
else
    bad "ubifs --kernel=FILE alone must be refused, rc=$RC log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- stage 2: settings that cannot be copied stop the run before any volume changes
reset_env; ubi_setup ubifs
RUN_ENV="$S2 update_rootfs=1 rootfs_file=$UFS STUB_UBIFS_RO_FAIL=1"
run
if printf '%s' "$OUT" | grep -q "Could not read the settings" && ! grep -qE "ubirmvol|ubirsvol|ubiupdatevol|ubimkvol" "$SB/tmp/flash.log" && rebooted; then
    ok "stage 2: unreadable settings stop the run before any volume is touched"
else
    bad "stage 2 settings read failure, log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# --- the unpack leaves rootfs.ubi (fresh-install image) in the archive
# An image that passes, so the run reaches the hand-off with what it unpacked
# still in /tmp: a refused one takes its unpack back (discard_unpacked).
reset_env; ubi_setup ubifs
make_fit_soc "$UFIT" gk7205v500
STUB_PIVOT_RC=0
stage="$SB/stage"; rm -rf "$stage"; mkdir -p "$stage"
cp "$UFIT" "$UFS" "$stage/"
dd if=/dev/zero bs=1k count=64 of="$stage/rootfs.ubi.gk7205v500" 2>/dev/null
(cd "$stage" && for f in fitImage.gk7205v500 rootfs.ubifs.gk7205v500 rootfs.ubi.gk7205v500; do
    md5sum "$f" > "$f.md5sum"; done && tar cf - . | gzip > "$SB/tmp/fw.tgz")
rm -f "$UFIT" "$UFS"
run -z --archive="$SB/tmp/fw.tgz"
if [ -f "$UFIT" ] && [ -f "$UFS" ] && [ ! -e "$SB/tmp/rootfs.ubi.gk7205v500" ] &&
    ! printf '%s' "$OUT" | grep -q "Wrong checksum"; then
    ok "ubifs: the volume images are unpacked, rootfs.ubi and its checksum are not"
else
    bad "ubifs unpack: fit=$([ -f "$UFIT" ] && echo y) ubifs=$([ -f "$UFS" ] && echo y) ubi=$([ -e "$SB/tmp/rootfs.ubi.gk7205v500" ] && echo y) out='$OUT'"
fi

# ---------------------------------------------------------------------------
echo
# --- the split NOR layout ----------------------------------------------------
# One "firmware" partition from the firmware area (0x50000, or after a larger
# boot area) to the end of the chip; the kernel divides it at boot into the
# uImage, the squashfs after it and rootfs_data from the squashfs end. sysupgrade writes firmware.bin through mtdblock and stages
# the settings at the new overlay start, where /init restores them. The
# mtdblock "devices" are sparse files here, so a test reads back what landed
# where.

# make_split_fw <file> <soc> <kernel blocks> <squashfs bytes_used>: a uImage
# named for the SoC whose header and data end inside its last 64 KiB block,
# then the squashfs at the next block, as post-image.sh packs firmware.bin.
make_split_fw() {
    local size=$(($3 * 65536 - 64 - 100))
    printf '\x27\x05\x19\x56' > "$1"
    dd if=/dev/zero bs=1 count=8 >> "$1" 2>/dev/null                  # 4..11
    printf %b "$(printf '\\x%02x\\x%02x\\x%02x\\x%02x' $(((size >> 24) & 255)) \
        $(((size >> 16) & 255)) $(((size >> 8) & 255)) $((size & 255)))" >> "$1"  # 12..15
    dd if=/dev/zero bs=1 count=16 >> "$1" 2>/dev/null                 # 16..31
    printf 'Linux-4.9.37-%s' "$2" >> "$1"                              # 32.. name
    truncate -s $(($3 * 65536)) "$1"
    make_squashfs "$SB/tmp/split.sqfs" "$4"
    cat "$SB/tmp/split.sqfs" >> "$1"; rm -f "$SB/tmp/split.sqfs"
    truncate -s $((($(stat -c %s "$1") + 65535) / 65536 * 65536)) "$1"
}

# set_split_mtd <layout> <chip bytes> [base]: /proc/mtd, /sys/class/mtd and
# the mtdblock files for "split" (firmware and the children the kernel made) or
# "fixed" (kernel 2M, rootfs 5M, rootfs_data to the end, plus the overlapping
# firmware alias the fixed-slot command line also carries), with the firmware
# area at base (0x50000: 256K boot, 64K env; else the boot area and a 256K env
# before it). The children of the split layout sit at the offsets an image of
# 30 + 40 blocks gives.
set_split_mtd() {
    local chip=$2 b=$((${3:-0x50000})) i=0 spec name off size env
    rm -rf "$SB/sys/class/mtd" "$SB/dev"; mkdir -p "$SB/sys/class/mtd" "$SB/dev"
    printf 'dev:    size   erasesize  name\n' > "$SB/proc/mtd"
    if [ "$b" -eq $((0x50000)) ]; then env=0x10000; else env=0x40000; fi
    if [ "$1" = split ]; then
        set -- u-boot:0:$((b - env)) env:$((b - env)):$env firmware:$b:$((chip - b)) \
            kernel:$b:0x1e0000 rootfs:$((b + 0x1e0000)):0x280000 \
            rootfs_data:$((b + 0x460000)):$((chip - b - 0x460000))
    else
        set -- u-boot:0:$((b - env)) env:$((b - env)):$env kernel:$b:0x200000 \
            rootfs:$((b + 0x200000)):0x500000 firmware:$b:0x700000 \
            rootfs_data:$((b + 0x700000)):$((chip - b - 0x700000))
    fi
    for spec in "$@"; do
        name=${spec%%:*}; spec=${spec#*:}; off=$((${spec%%:*})); size=$((${spec#*:}))
        mkdir -p "$SB/sys/class/mtd/mtd$i" "$SB/sys/class/mtd/mtd${i}ro"
        echo "$name" > "$SB/sys/class/mtd/mtd$i/name"
        echo "$off" > "$SB/sys/class/mtd/mtd$i/offset"
        echo "$size" > "$SB/sys/class/mtd/mtd$i/size"
        printf 'mtd%d: %08x 00010000 "%s"\n' "$i" "$size" "$name" >> "$SB/proc/mtd"
        truncate -s "$size" "$SB/dev/mtdblock$i"
        i=$((i + 1))
    done
}

# split_env <layout> [chip] [base]: a gk7205v200 running from that layout, its overlay
# mounted in the sandbox with a marker in it, on a bootloader that boots the
# split layout.
split_env() {
    reset_env
    set_platform gk7205v200_lite
    STUB_SOC=gk7205v200; STUB_IMG_SOC=gk7205v200; STUB_VENDOR=goke
    set_split_mtd "$1" "${2:-0x800000}" "${3:-0x50000}"
    # What OpenIPC U-Boot passes on the split layout: nothing about the flash.
    set_cmdline 'mem=32M console=ttyAMA0,115200 panic=20 rootfstype=squashfs init=/init mmz_allocator=gk'
    rm -rf "$SB/overlay"; mkdir -p "$SB/overlay/etc"; echo kept > "$SB/overlay/etc/marker"
    printf 'tmpfs %s/tmp tmpfs rw 0 0\n/dev/mtdblock5 %s/overlay jffs2 rw 0 0\n' "$SB" "$SB" \
        > "$SB/proc/mounts"
    # The MTD character devices read the same files: here the chip is what
    # mtdblock wrote.
    RUN_ENV="MTD_SYS=$SB/sys/class/mtd MTDBLOCK=$SB/dev/mtdblock MTDCHR=$SB/dev/mtdblock"
    make_split_fw "$SB/tmp/firmware.bin.gk7205v200" gk7205v200 "${SPLIT_FIT_BLOCKS:-30}" \
        "${SPLIT_SQ_USED:-$((40 * 65536 - 4000))}"
    make_archive "$SB/tmp/firmware.bin.gk7205v200"
}
# The bytes at absolute flash offset $2 in mtdblock$1, which starts at $3.
flash_at() { xxd -p -l "$4" -s $(($2 - $3)) "$SB/dev/mtdblock$1"; }
# The settings blob at absolute offset $2 in mtdblock$1 (starting at $3): its
# length field, and the files of the tar it carries.
blob_len() { dd if="$SB/dev/mtdblock$1" bs=1 skip=$(($2 - $3 + 12)) count=12 2>/dev/null; }
blob_files() {
    local n=$((10#$(blob_len "$@")))
    dd if="$SB/dev/mtdblock$1" bs=1 skip=$(($2 - $3 + 24)) count="$n" 2>/dev/null | tar tzf - 2>/dev/null
}
FW_HEAD=$(printf '27051956%08x' 0)
OVL=$((0x50000 + 70 * 65536))

split_env split
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ] &&
    [ "$(flash_at 2 $((0x50000 + 30 * 65536)) 0x50000 4)" = 68737173 ] &&
    [ "$(dd if="$SB/dev/mtdblock2" bs=1 skip=$((OVL - 0x50000)) count=12 2>/dev/null)" = OPENIPC-OVL1 ] &&
    blob_files 2 "$OVL" 0x50000 | grep -q 'etc/marker' && nothing_wrote && rebooted; then
    ok "split layout: firmware.bin at 0x50000, the settings staged where rootfs_data will start"
else
    bad "split layout upgrade, rc=$RC head=$(flash_at 2 0x50000 0x50000 8) out='$OUT'"
fi
if printf '%s' "$OUT" | grep -q "Settings staged at $(printf 0x%x "$OVL")"; then
    ok "split layout: the overlay offset is the uImage and the squashfs, each to 64 KiB"
else
    bad "split layout: expected the settings at $(printf 0x%x "$OVL"), out='$OUT'"
fi

# -n: a blob of length 0, which /init takes as "erase, restore nothing".
# A profile camera on the split layout: firmware.bin's device profile is in
# its squashfs, which nothing on the split path read (#2689 review). Another
# profile's image is refused before anything is written; its own is taken.
split_env split
set_platform gk7205v200_lite_acme-cam1
STUB_IMG_PLATFORM=gk7205v200_lite_other-cam2
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -qF "Wrong platform!" &&
    ! printf '%s' "$OUT" | grep -qF "Protected: flashing continues" &&
    [ "$(flash_at 2 0x50000 0x50000 8)" != "$FW_HEAD" ]; then
    ok "split layout: another device profile's firmware.bin is refused before the pivot"
else
    bad "split layout, other profile: rc=$RC out='$OUT'"
fi
split_env split
set_platform gk7205v200_lite_acme-cam1
STUB_IMG_PLATFORM=gk7205v200_lite_acme-cam1
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ]; then
    ok "split layout: the camera's own profile's firmware.bin is written"
else
    bad "split layout, own profile: rc=$RC out='$OUT'"
fi

split_env split
run -z -n --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(blob_len 2 "$OVL" 0x50000)" = 000000000000 ]; then
    ok "split layout -n: an empty settings blob, so the first boot starts clean"
else
    bad "split layout -n, rc=$RC len='$(blob_len 2 "$OVL" 0x50000)' out='$OUT'"
fi

# Handed off: the RAM phase runs as PID 1, release_old_root has let go of the
# old root and the overlay's mount with it, so /proc/mounts no longer shows
# rootfs_data. The settings are still on it, and must still be carried: an
# empty blob here is the camera starting from defaults after every NOR upgrade
# since the hand-off (#2648).
split_handed_off() {
    split_env split
    printf 'tmpfs %s/tmp tmpfs rw 0 0\n' "$SB" > "$SB/proc/mounts"
    # What is on rootfs_data (mtdblock5): the settings, unlike anything the
    # stub could make up.
    rm -rf "$SB/jffs2"; mkdir -p "$SB/jffs2/mtdblock5/etc"
    echo "from-rootfs_data" > "$SB/jffs2/mtdblock5/etc/carried"
    RUN_ENV="$RUN_ENV STUB_JFFS2_DIR=$SB/jffs2 _ramfs_phase=1 _handoff=1 split_overlay_mounted=1 image_combined=1 update_kernel=1 update_rootfs=1 model=gk7205v200 skip_soc=1 skip_ver=1 root_on_flash=1 ram_root_shipped=1"
    tar -xzf "$SB/tmp/fw.tgz" -C "$SB/tmp"
}
split_handed_off
run
u=$(logged_at "umount -l /mnt"); m=$(logged_at "mount jffs2 ro")
if [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ] &&
    blob_files 2 "$OVL" 0x50000 | grep -q 'etc/carried' &&
    grep -q "mount jffs2 ro .*mtdblock5 " "$SB/tmp/flash.log" &&
    [ -n "$u" ] && [ -n "$m" ] && [ "$u" -lt "$m" ] && rebooted; then
    ok "split layout, handed off: the overlay is mounted read-only and the settings carried"
else
    bad "split layout handed off must carry the settings, len='$(blob_len 2 "$OVL" 0x50000)' log='$(cat "$SB/tmp/flash.log")' out='$OUT'"
fi

# The overlay cannot be read: refused before the write, not upgraded to defaults.
split_handed_off
STUB_JFFS2_RO_FAIL=1 run
if printf '%s' "$OUT" | grep -q "Could not read the settings" &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && ! printf '%s' "$OUT" | grep -q "Firmware updated"; then
    ok "split layout, handed off: an overlay that will not mount stops the run, nothing written"
else
    bad "split layout handed off, unreadable overlay must refuse, rc=$RC out='$OUT'"
fi

# The copy is made but the partition will not let go: refused before the write.
split_handed_off
STUB_OVL_UMOUNT_FAIL=1 run
if printf '%s' "$OUT" | grep -q "Could not release the settings partition" &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && ! printf '%s' "$OUT" | grep -q "Firmware updated"; then
    ok "split layout, handed off: a settings mount that will not unmount stops the run, nothing written"
else
    bad "split layout handed off, stuck settings mount must refuse, rc=$RC out='$OUT'"
fi

# -n handed off: nothing to carry, nothing mounted.
split_handed_off
RUN_ENV="$RUN_ENV clear_overlay=1"
run
if [ "$(blob_len 2 "$OVL" 0x50000)" = 000000000000 ] && ! grep -q "mount jffs2" "$SB/tmp/flash.log"; then
    ok "split layout, handed off, -n: an empty blob, the overlay left unmounted"
else
    bad "split layout handed off -n, len='$(blob_len 2 "$OVL" 0x50000)' out='$OUT'"
fi

# The first phase records that there are settings, for the RAM phase to look for.
split_env split
STUB_PIVOT_RC=0
run -z --archive="$SB/tmp/fw.tgz"
if grep -q "^kill -QUIT 1" "$SB/tmp/flash.log" &&
    grep -q "split_overlay_mounted=.1" "$SB/ram/sysupgrade.env" 2>/dev/null; then
    ok "split layout: the hand-off carries that the overlay was mounted"
else
    bad "split layout hand-off must record the mounted overlay, log='$(cat "$SB/tmp/flash.log")' env='$(grep overlay "$SB/ram/sysupgrade.env" 2>/dev/null)' out='$OUT'"
fi

# From fixed slots: the image runs on from the kernel slot into the rootfs one,
# and the settings land in the old rootfs, where the new rootfs_data starts.
split_env fixed
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ] &&
    [ "$(flash_at 2 $((0x50000 + 30 * 65536)) 0x50000 4)" = 68737173 ] &&
    [ "$(dd if="$SB/dev/mtdblock3" bs=1 skip=$((OVL - 0x250000)) count=12 2>/dev/null)" = OPENIPC-OVL1 ] &&
    blob_files 3 "$OVL" 0x250000 | grep -q 'etc/marker' &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock4")" = 00000000 ] && nothing_wrote; then
    ok "fixed slots -> split: written across kernel and rootfs, the firmware alias left alone"
else
    bad "fixed -> split, rc=$RC out='$OUT'"
fi

# A 3.x kernel gives no partition offsets in sysfs (they came in 4.1): the
# 3.18 of the Hi3516CV300 and Hi3519V101 families and Hi3536CV100, the 3.0
# and 3.4 of the oldest SoCs. The offsets then follow from the partition
# order, and are taken once the kernel and rootfs partitions hold a uImage and
# a squashfs, as on a running camera.
no_sysfs_offsets() {
    rm -f "$SB"/sys/class/mtd/mtd*/offset
    [ -n "${1:-}" ] && printf '\x27\x05\x19\x56' | dd of="$SB/dev/mtdblock$1" conv=notrunc 2>/dev/null
    [ -n "${2:-}" ] && printf 'hsqs' | dd of="$SB/dev/mtdblock$2" conv=notrunc 2>/dev/null
    return 0
}
split_env fixed
no_sysfs_offsets 2 3
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ] &&
    [ "$(flash_at 2 $((0x50000 + 30 * 65536)) 0x50000 4)" = 68737173 ] &&
    [ "$(dd if="$SB/dev/mtdblock3" bs=1 skip=$((OVL - 0x250000)) count=12 2>/dev/null)" = OPENIPC-OVL1 ] &&
    blob_files 3 "$OVL" 0x250000 | grep -q 'etc/marker' &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock4")" = 00000000 ] && nothing_wrote; then
    ok "fixed slots -> split without sysfs offsets (3.x): the offsets from the order, the settings carried"
else
    bad "fixed -> split without sysfs offsets, rc=$RC out='$OUT'"
fi

split_env split
no_sysfs_offsets 3 4
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ] &&
    [ "$(dd if="$SB/dev/mtdblock2" bs=1 skip=$((OVL - 0x50000)) count=12 2>/dev/null)" = OPENIPC-OVL1 ] &&
    blob_files 2 "$OVL" 0x50000 | grep -q 'etc/marker' && rebooted; then
    ok "split layout without sysfs offsets (3.x): firmware.bin at 0x50000, the settings staged"
else
    bad "split layout without sysfs offsets, rc=$RC out='$OUT'"
fi

# Some offsets in sysfs and some not: still checked as worked out.
split_env fixed
rm -f "$SB"/sys/class/mtd/mtd3/offset
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && nothing_wrote; then
    ok "one offset missing from sysfs: the images are checked too, and an empty kernel refuses"
else
    bad "a partly worked-out layout must be checked, rc=$RC out='$OUT'"
fi

# A layout handed in through the environment is not taken.
split_env split
RUN_ENV="$RUN_ENV nor_split_layout=mtd0"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ] &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock0")" = 00000000 ]; then
    ok "a nor_split_layout from the environment is ignored: firmware.bin where the flash says"
else
    bad "an inherited nor_split_layout must not steer the write, rc=$RC out='$OUT'"
fi

# Offsets from the order are not taken when the partitions do not hold what
# their names say: nothing is written.
split_env fixed
no_sysfs_offsets
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && nothing_wrote; then
    ok "without sysfs offsets, a kernel partition with no uImage refuses the upgrade, nothing written"
else
    bad "derived offsets over unrecognised partitions must refuse, rc=$RC out='$OUT'"
fi

# The same under an older bootloader, which spells out the fixed slots: any
# U-Boot that loads the kernel from 0x50000 boots the split layout, since the
# kernel finds its own partitions, so nothing about the bootloader is asked.
split_env fixed
set_cmdline 'mem=32M console=ttyAMA0,115200 panic=20 rootfstype=squashfs root=/dev/mtdblock3 init=/init mtdparts=sfc:256k(boot),64k(env),2048k(kernel),5120k(rootfs),-(rootfs_data) mmz_allocator=gk'
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ] && rebooted; then
    ok "split image under an older bootloader's fixed-slot command line: installed"
else
    bad "split image under an older bootloader must install, rc=$RC out='$OUT'"
fi

# An image that leaves no block for the settings is refused before the write.
SPLIT_SQ_USED=$((93 * 65536)) split_env split
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "settings do not fit" &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && nothing_wrote; then
    ok "split layout: an image that fills the flash is refused, nothing written"
else
    bad "split layout: an oversized image must be refused, rc=$RC out='$OUT'"
fi

# The gk710x kernel names its uImage "Linux-3.4.43-Goke-gk7102": the SoC is the
# last word of the name, not the third.
split_env split
printf 'Linux-3.4.43-Goke-gk7205v200\0' | dd of="$SB/tmp/firmware.bin.gk7205v200" bs=1 seek=32 conv=notrunc 2>/dev/null
make_archive "$SB/tmp/firmware.bin.gk7205v200"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 4)" = 27051956 ] && rebooted; then
    ok "split layout: a uImage named Linux-<version>-<vendor>-<soc> is taken for its SoC"
else
    bad "a vendor word in the uImage name must not hide the SoC, rc=$RC out='$OUT'"
fi

# Another SoC's image: refused before anything is written.
split_env split
make_split_fw "$SB/tmp/firmware.bin.gk7205v200" gk7205v300 30 $((40 * 65536 - 4000))
make_archive "$SB/tmp/firmware.bin.gk7205v200"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && nothing_wrote &&
    ! rebooted; then
    ok "split layout: another SoC's firmware.bin is refused before the pivot, nothing written, no reboot"
else
    bad "split layout: wrong SoC must be refused, rc=$RC out='$OUT'"
fi
# Bytes past the squashfs: not what post-image.sh packs, and they would run
# into the settings. Refused before anything moves.
split_env split
head -c 131072 /dev/urandom >> "$SB/tmp/firmware.bin.gk7205v200"
make_archive "$SB/tmp/firmware.bin.gk7205v200"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "runs past the end of its squashfs" &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && nothing_wrote && ! rebooted; then
    ok "split layout: a firmware.bin with bytes past its squashfs is refused, no reboot"
else
    bad "split layout: trailing bytes must be refused, rc=$RC out='$OUT'"
fi

# Settings that do not fit what the image leaves: refused before the pivot.
SPLIT_SQ_USED=$((92 * 65536)) split_env split
head -c 200000 /dev/urandom > "$SB/overlay/etc/noise"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "settings do not fit" &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && nothing_wrote && ! rebooted; then
    ok "split layout: settings too big for the space left are refused before the pivot"
else
    bad "split layout: oversized settings must be refused unwritten, rc=$RC out='$OUT'"
fi

# Any other combined image on a camera already on the split layout: its
# "firmware" partition is the whole flash after env.
split_env split
make_combined "$SB/tmp/firmware.bin.gk7205v200"
make_archive "$SB/tmp/firmware.bin.gk7205v200"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "not a split-layout firmware.bin" &&
    [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && nothing_wrote && ! rebooted; then
    ok "split layout: a combined image of another shape is refused, nothing written"
else
    bad "split layout: a non-split combined image must be refused, rc=$RC out='$OUT'"
fi

# What mtdblock accepted is not what reached the chip (its cache was never
# written out): the read back through the MTD character device says so, and the
# run does not report the firmware written.
split_env split
mkdir -p "$SB/stale"
for i in 0 1 2 3 4 5; do truncate -s "$(stat -c %s "$SB/dev/mtdblock$i")" "$SB/stale/mtd$i"; done
RUN_ENV="$RUN_ENV MTDCHR=$SB/stale/mtd"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "does not read back as the image" &&
    ! printf '%s' "$OUT" | grep -q "Firmware updated"; then
    ok "split layout: a write that never reached the chip is caught by the read-back"
else
    bad "split layout: a stale chip must fail the write, rc=$RC out='$OUT'"
fi
rm -rf "$SB/stale"

# The write reaches the flash the camera runs from, so the run reboots whatever
# happens after it started: done (-x included, which otherwise skips the
# reboot), or failed half way -- a half-written image is no more bootable than
# an erased one.
split_env split
run -z --archive="$SB/tmp/fw.tgz" -x
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0x50000 0x50000 8)" = "$FW_HEAD" ] && rebooted; then
    ok "split layout -x: written, and the run reboots anyway"
else
    bad "split layout -x: expected the image written and a reboot, rc=$RC out='$OUT'"
fi
split_env split
mkdir -p "$SB/stale"
for i in 0 1 2 3 4 5; do truncate -s "$(stat -c %s "$SB/dev/mtdblock$i")" "$SB/stale/mtd$i"; done
RUN_ENV="$RUN_ENV MTDCHR=$SB/stale/mtd"
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -ne 0 ] && rebooted; then
    ok "split layout: a write that started and failed still reboots"
else
    bad "split layout: a failed write must reboot, rc=$RC out='$OUT'"
fi
rm -rf "$SB/stale"

# The firmware area starts where the board puts it: a boot area larger than
# 256K (hi3519dv500's signed boot image needs 512K, then a 256K environment)
# puts it at 0xc0000, and the image and the settings land relative to that.
split_env split 0x1000000 0xc0000
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && printf '%s' "$OUT" | grep -q "firmware.bin at 0xc0000" &&
    [ "$(flash_at 2 0xc0000 0xc0000 8)" = "$FW_HEAD" ] &&
    [ "$(dd if="$SB/dev/mtdblock2" bs=1 skip=$((70 * 65536)) count=12 2>/dev/null)" = OPENIPC-OVL1 ]; then
    ok "split layout at 0xc0000: firmware.bin there, the settings 70 blocks after it"
else
    bad "split layout at 0xc0000: expected the image at 0xc0000, rc=$RC out='$OUT'"
fi
split_env fixed 0x1000000 0xc0000
run -z --archive="$SB/tmp/fw.tgz"
if [ "$RC" -eq 0 ] && [ "$(flash_at 2 0xc0000 0xc0000 8)" = "$FW_HEAD" ]; then
    ok "fixed slots at 0xc0000 -> split: written where the kernel slot starts"
else
    bad "fixed slots at 0xc0000: expected the image at 0xc0000, rc=$RC out='$OUT'"
fi

# A camera on the split layout has no slots for a separate kernel and rootfs.
split_env split
make_uimage "$SB/tmp/uImage.gk7205v200" gk7205v200
make_rootfs "$SB/tmp/rootfs.squashfs.gk7205v200"
rm -f "$SB/tmp/firmware.bin.gk7205v200"
run -z --kernel="$SB/tmp/uImage.gk7205v200" --rootfs="$SB/tmp/rootfs.squashfs.gk7205v200"
if [ "$RC" -ne 0 ] && printf '%s' "$OUT" | grep -q "openipc.gk7205v200-nor-lite-split.tgz" &&
    nothing_wrote && [ "$(xxd -p -l 4 "$SB/dev/mtdblock2")" = 00000000 ] && ! rebooted; then
    ok "split layout: a separate kernel and rootfs are refused, the -split archive named"
else
    bad "split layout: kernel+rootfs must be refused, rc=$RC out='$OUT'"
fi
unset STUB_SOC STUB_IMG_SOC STUB_VENDOR
rm -rf "$SB/overlay" "$SB/dev"

# The default URL: the -split archive wherever the release publishes one for the
# camera, whatever its SoC or bootloader; the plain package otherwise.
default_url_is "$F/openipc.gk7205v200-nor-lite-split.tgz" \
    eval 'set_platform gk7205v200_lite; STUB_SOC=gk7205v200; STUB_SPLIT_PUBLISHED=1'
default_url_is "$F/openipc.hi3516cv6xx-nor-lite-split.tgz" \
    eval 'set_platform hi3516cv6xx_lite; STUB_SOC=hi3516cv6xx; STUB_SPLIT_PUBLISHED=1'
# dev and glibc live in the builder repo; a split-only board publishes -split
# there too (hi3516ev300, hi3518ev300), and the camera must ask for it.
default_url_is "$B/openipc.hi3516ev300-nor-dev-split.tgz" \
    eval 'set_platform hi3516ev300_dev dev; STUB_SOC=hi3516ev300; STUB_SPLIT_PUBLISHED=1'
unset STUB_SPLIT_PUBLISHED
default_url_is "$B/openipc.hi3516ev300-nor-dev.tgz" \
    eval 'set_platform hi3516ev300_dev dev; STUB_SOC=hi3516ev300'

default_url_is "$F/openipc.ssc338q-nor-lite.tgz" true
# --channel resolves the plain package from the manifest; the family fetches
# the -split one beside it. A board that publishes only the split package
# (hi3516cv6xx) is listed under nor_split, and that is what it resolves to.
printf '@channel stable nightly-1\nnightly-1 gk7205v200_lite nor %s\nnightly-1 ssc338q_lite nor %s\nnightly-1 hi3516cv6xx_lite nor_split %s\n' \
    "$F/openipc.gk7205v200-nor-lite.tgz" "$F/openipc.ssc338q-nor-lite.tgz" \
    "$F/openipc.hi3516cv6xx-nor-lite-split.tgz" > "$SB/manifest.flat"
for c in gk7205v200 ssc338q hi3516cv6xx; do
    reset_env; set_meminfo 8192; STUB_DL_BYTES=8691055
    set_platform ${c}_lite; STUB_SOC=$c
    STUB_SPLIT_PUBLISHED=; [ "$c" = gk7205v200 ] && STUB_SPLIT_PUBLISHED=1
    RUN_ENV="STUB_MANIFEST=$SB/manifest.flat"
    run -z --channel=stable
    want="$F/openipc.$c-nor-lite.tgz"
    [ "$c" = ssc338q ] || want="$F/openipc.$c-nor-lite-split.tgz"
    if printf '%s' "$OUT" | grep -qF "Download from $want" && nothing_wrote; then
        ok "--channel, $c -> $want"
    else
        bad "--channel, $c: expected 'Download from $want', rc=$RC out='$OUT'"
    fi
done
rm -f "$SB/manifest.flat"
unset STUB_SOC STUB_SPLIT_PUBLISHED

# --- the from -> to block and the history (#2676) ---------------------------
# Users upgrade nightly by nightly and, once a build misbehaves, cannot say
# which one they came from. The block has to name both builds and the way
# back BEFORE the first write, and the history has to keep the line.
HIST="$SB/etc/sysupgrade.history"
banner_case() { reset_env; rm -f "$SB/tmp/manifest.flat"; STUB_IMG_BUILD_ID=nightly-20260711-bbbbbbb; }
# The same two images, downloaded by `-k -r` from the default URL: a run that
# is online, so it may ask the manifest about the build being left.
banner_dl() {
    make_archive "$K" "$R"
    mkdir -p "$SB/keep" && mv "$SB/tmp/fw.tgz" "$SB/keep/dl.tgz"
    rm -f "$SB"/tmp/*.ssc338q
    STUB_DL_FILE="$SB/keep/dl.tgz"
}

banner_case
run -z --kernel="$K" --rootfs="$R"
b=$(at "UPDATING: nightly-20260601-aaaaaaa (2026.06.01)"); k=$(at "Update kernel from")
if [ "$RC" -eq 0 ] && [ -n "$b" ] && [ -n "$k" ] && [ "$b" -lt "$k" ] &&
    printf '%s' "$OUT" | grep -qF "TO: nightly-20260711-bbbbbbb (2026.07.11)" &&
    printf '%s' "$OUT" | grep -qF "sysupgrade --build=nightly-20260601-aaaaaaa" &&
    printf '%s' "$OUT" | grep -qF "Daily builds are not release-tested"; then
    ok "from -> to block names both builds and the way back, before the first write"
else
    bad "from -> to block: expected before the kernel write (block@${b:-none} kernel@${k:-none}), rc=$RC out='$OUT'"
fi
if [ "$(wc -l < "$HIST" 2>/dev/null)" = 1 ] &&
    grep -qE '^[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9:]{8}Z nightly-20260601-aaaaaaa nightly-20260711-bbbbbbb cli$' "$HIST"; then
    ok "the upgrade is recorded in the history: time, from, to, door"
else
    bad "history: expected one 'time from to cli' line, got '$(cat "$HIST" 2>/dev/null)'"
fi
run -z --history
if [ "$RC" -eq 0 ] && printf '%s' "$OUT" | grep -qF "nightly-20260601-aaaaaaa nightly-20260711-bbbbbbb cli" &&
    nothing_wrote; then
    ok "--history prints the record and writes nothing"
else
    bad "--history: rc=$RC out='$OUT'"
fi
rm -f "$HIST"
run -z --history
if [ "$RC" -eq 0 ] && printf '%s' "$OUT" | grep -qF "No upgrades recorded"; then
    ok "--history on a camera with no record says so"
else
    bad "--history, no record: rc=$RC out='$OUT'"
fi

# The history stays bounded on a camera upgraded nightly for years.
banner_case
for i in $(seq 1 60); do echo "2026-01-01T00:00:00Z old-$i old-$((i + 1)) cli"; done > "$HIST"
run -z --kernel="$K" --rootfs="$R"
if [ "$(wc -l < "$HIST")" = 50 ] && grep -q "^.* old-12 " "$HIST" && ! grep -q " old-11 " "$HIST" &&
    tail -1 "$HIST" | grep -q " nightly-20260711-bbbbbbb cli$"; then
    ok "history keeps the last 50 upgrades"
else
    bad "history cap: $(wc -l < "$HIST") lines, head '$(head -1 "$HIST")', tail '$(tail -1 "$HIST")'"
fi

# The rollback line is only as good as the manifest: a build pruned from it
# cannot be reached by --build, and saying otherwise is worse than silence.
banner_case; banner_dl
printf 'nightly-20260711-bbbbbbb ssc338q_lite nor x\nnightly-20260601-aaaaaaa ssc338q_lite nor x\n' > "$SB/manifest.flat"
RUN_ENV="STUB_MANIFEST=$SB/manifest.flat"
run -z -k -r
if printf '%s' "$OUT" | grep -qF "return to this one with" &&
    printf '%s' "$OUT" | grep -qF "sysupgrade --build=nightly-20260601-aaaaaaa"; then
    ok "rollback line: the build being left is in the manifest"
else
    bad "rollback line, listed build: out='$OUT'"
fi
banner_case; banner_dl
printf 'nightly-20260711-bbbbbbb ssc338q_lite nor x\n' > "$SB/manifest.flat"
RUN_ENV="STUB_MANIFEST=$SB/manifest.flat"
run -z -k -r
if printf '%s' "$OUT" | grep -qF "nightly-20260601-aaaaaaa is no longer in the manifest" &&
    ! printf '%s' "$OUT" | grep -qF "sysupgrade --build=nightly-20260601-aaaaaaa" && flashed /dev/mtd3; then
    ok "rollback line: a pruned build is not offered, and the upgrade goes on"
else
    bad "rollback line, pruned build: rc=$RC out='$OUT'"
fi
# A run from local files may be offline, with its services already stopped:
# it does not wait on the network for the rollback line, and says the
# command holds only while the build is published.
banner_case
RUN_ENV="STUB_MANIFEST=$SB/manifest.flat"
run -z --kernel="$K" --rootfs="$R"
if [ ! -s "$SB/tmp/manifest.flat" ] && printf '%s' "$OUT" | grep -qF "is still published" &&
    flashed /dev/mtd3; then
    ok "rollback line: local files do not fetch the manifest"
else
    bad "rollback line, local files: manifest='$(cat "$SB/tmp/manifest.flat" 2>/dev/null)' out='$OUT'"
fi
rm -f "$SB/manifest.flat" "$SB/tmp/manifest.flat"

# The history records transitions. Staying on the running build is not one:
# the flash phase skips a same-version image, and a forced reflash of the
# same build changes nothing a user would roll back from.
banner_case
STUB_IMG_BUILD_ID=nightly-20260601-aaaaaaa
STUB_IMG_VERSION=2026.06.01
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && ! flashed /dev/mtd3 && [ ! -e "$HIST" ] &&
    ! printf '%s' "$OUT" | grep -qF "sysupgrade --history"; then
    ok "same build: nothing recorded"
else
    bad "same build: rc=$RC history='$(cat "$HIST" 2>/dev/null)' out='$OUT'"
fi

# A kernel-only run leaves the rootfs, and so the build the camera reports,
# where it was: it must not claim the camera moved to another build.
banner_case
run -z --kernel="$K"
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && ! flashed /dev/mtd3 &&
    printf '%s' "$OUT" | grep -qF "TO: nightly-20260601-aaaaaaa, with the kernel from" &&
    tail -1 "$HIST" | grep -q " nightly-20260601-aaaaaaa kernel:uImage.ssc338q cli$"; then
    ok "kernel only: the build stays, the kernel is named"
else
    bad "kernel only: rc=$RC history='$(cat "$HIST" 2>/dev/null)' out='$OUT'"
fi

# builder lists a device profile under the chip it was built for: a
# gk7205v210 profile is gk7205v210_<...> in its manifest, while $model is
# aliased to gk7205v200. Looked up under the alias alone, every build it has
# read as pruned, and --list-builds came back empty.
reset_env; rm -f "$SB/tmp/manifest.flat"
set_platform gk7205v210_lite_acme-cam1
printf 'nightly-20260711-bbbbbbb gk7205v210_lite_acme-cam1 nor x\nnightly-20260601-aaaaaaa gk7205v210_lite_acme-cam1 nor x\n' > "$SB/manifest.flat"
RUN_ENV="STUB_MANIFEST=$SB/manifest.flat"
run -z --list-builds
if [ "$RC" -eq 0 ] && printf '%s' "$OUT" | grep -qF "* nightly-20260601-aaaaaaa" &&
    printf '%s' "$OUT" | grep -qF "nightly-20260711-bbbbbbb"; then
    ok "aliased builder profile: its builds are found under its own platform"
else
    bad "aliased builder profile, --list-builds: rc=$RC out='$OUT'"
fi
rm -f "$SB/manifest.flat" "$SB/tmp/manifest.flat"

# A build that was never published has no --build to offer.
banner_case
sed -i 's/^BUILD_ID=.*/BUILD_ID=local-20260601-aaaaaaa/' "$SB/etc/os-release"
run -z --kernel="$K" --rootfs="$R"
if printf '%s' "$OUT" | grep -qF "never published" && ! printf '%s' "$OUT" | grep -qF -- "--build=local-"; then
    ok "rollback line: a local build is not offered to --build"
else
    bad "rollback line, local build: out='$OUT'"
fi

# An image this kernel cannot mount still flashes; the block says what it
# could not read rather than guessing, and the history keeps "unknown".
banner_case
STUB_MOUNT=fail
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 &&
    printf '%s' "$OUT" | grep -qF "TO: unknown (not readable: this kernel cannot mount the image)" &&
    tail -1 "$HIST" | grep -q " nightly-20260601-aaaaaaa unknown cli$"; then
    ok "unmountable image: target reported unknown, flash proceeds"
else
    bad "unmountable image: rc=$RC out='$OUT' history='$(cat "$HIST" 2>/dev/null)'"
fi

# A foreign image is refused at the mount that reads it, before the pivot --
# where verify_rootfs would have had to reboot to refuse it -- and before the
# history records an upgrade that will not happen (a lab gk7205v210 builder
# profile did both).
banner_case
STUB_IMG_SOC=gk7205v300
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote && [ ! -e "$HIST" ] &&
    printf '%s' "$OUT" | grep -qF "Wrong SoC!" &&
    ! printf '%s' "$OUT" | grep -qF "UPDATING:" &&
    ! printf '%s' "$OUT" | grep -qF "Protected: flashing continues"; then
    ok "wrong SoC: refused before the pivot, nothing recorded"
else
    bad "wrong SoC before the pivot: rc=$RC history='$(cat "$HIST" 2>/dev/null)' out='$OUT'"
fi
banner_case
set_platform ssc338q_lite_acme-cam1
STUB_IMG_PLATFORM=ssc338q_lite
run -z --kernel="$K" --rootfs="$R"
if [ "$RC" -ne 0 ] && nothing_wrote && [ ! -e "$HIST" ] &&
    printf '%s' "$OUT" | grep -qF "Wrong platform!" &&
    ! printf '%s' "$OUT" | grep -qF "Protected: flashing continues"; then
    ok "dropped device profile: refused before the pivot, nothing recorded"
else
    bad "dropped device profile before the pivot: rc=$RC history='$(cat "$HIST" 2>/dev/null)' out='$OUT'"
fi

# -n wipes the overlay the history lives on: nothing to record, and no
# promise that it was.
banner_case
run -z -n --kernel="$K" --rootfs="$R"
if [ ! -e "$HIST" ] && ! printf '%s' "$OUT" | grep -qF "sysupgrade --history"; then
    ok "-n records nothing, and does not claim to"
else
    bad "-n: history='$(cat "$HIST" 2>/dev/null)' out='$OUT'"
fi
reset_env

# --- a builder device profile on an aliased SoC (#2688) ----------------------
# builder builds a gk7205v210 profile for gk7205v210 and names and stamps its
# images so, while $model is aliased to gk7205v200: every upgrade of such a
# camera was refused with "No kernel image for this camera (gk7205v200)".
alias_case() {
    reset_env
    STUB_VENDOR=goke STUB_SOC=gk7205v210
    set_platform gk7205v210_lite_acme-cam1
    STUB_IMG_PLATFORM=gk7205v210_lite_acme-cam1
    rm -f "$SB"/tmp/*.gk7205v2?0
    make_uimage "$SB/tmp/uImage.$1" "$1"
    make_rootfs "$SB/tmp/rootfs.squashfs.$1"
    make_archive "$SB/tmp/uImage.$1" "$SB/tmp/rootfs.squashfs.$1"
    mkdir -p "$SB/keep" && mv "$SB/tmp/fw.tgz" "$SB/keep/dl.tgz"
    rm -f "$SB"/tmp/*.gk7205v2?0
    STUB_DL_FILE="$SB/keep/dl.tgz"
}

alias_case gk7205v210
STUB_IMG_SOC=gk7205v210
run -z -k -r
if [ "$RC" -eq 0 ] && flashed /dev/mtd2 && flashed /dev/mtd3 &&
    printf '%s' "$OUT" | grep -qF "Images named for gk7205v210" &&
    printf '%s' "$OUT" | grep -qF "SoC OK (gk7205v210, aliased to gk7205v200)"; then
    ok "aliased builder profile: images named for its own SoC are flashed"
else
    bad "aliased builder profile: expected both flashed, rc=$RC out='$OUT'"
fi

# Accepting the camera's own SoC is not accepting any SoC.
alias_case gk7205v210
STUB_IMG_SOC=gk7205v300
run -z -k -r
if [ "$RC" -ne 0 ] && nothing_wrote && printf '%s' "$OUT" | grep -qF "Wrong SoC!"; then
    ok "aliased builder profile: a foreign image in its archive is still refused"
else
    bad "aliased builder profile, foreign image: rc=$RC out='$OUT'"
fi

# An archive named for the canonical model is taken as it always was.
alias_case gk7205v200
STUB_IMG_SOC=gk7205v200
run -z -k -r
if [ "$RC" -eq 0 ] && flashed /dev/mtd3 && ! printf '%s' "$OUT" | grep -qF "Images named for"; then
    ok "aliased SoC, images named for the canonical model: unchanged"
else
    bad "aliased SoC, canonical names: rc=$RC out='$OUT'"
fi
rm -f "$SB"/tmp/*.gk7205v2?0
reset_env

echo "=== Part 2: invariants in $SRC ==="

# An option named in a user-facing message must exist in the parser.
# --connect-timeout, --speed-limit/--speed-time and --max-filesize are curl's,
# and --exclude is tar's, not ours.
for opt in $(grep -oE '\-\-[a-z_]+' "$SRC" | sort -u); do
    case "$opt" in
        --force_*|--wipe_overlay|--no_reboot|--no_update|--no_ramfs|--help|--web|--url|--archive|--kernel|--rootfs|--channel|--build|--list*|--history|--insecure|--connect*|--speed*|--proto*|--max*|--exclude) continue ;;
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
awk '/^restore_resources\(\)/,/^}/' "$SRC" | grep -q 'majestic_pending=restart' \
    && awk '/^restore_majestic\(\)/,/^}/' "$SRC" | grep -q 'S95majestic "$majestic_pending"' \
    && ok "restore_resources restarts majestic rather than starting it" \
    || bad "SIGQUIT leaves majestic running without an SDK; 'start' is a no-op, it needs 'restart'"
# ...and only once the lock is gone, which the supervisor waits for.
awk '/^die\(\)/,/^}/' "$SRC" | awk '/rm -f \$LOCK_FILE/ { l = NR } /restore_majestic/ { m = NR } END { exit !(l && m > l) }' \
    && ok "die() brings majestic back after releasing the lock" \
    || bad "die() must call restore_majestic after removing \$LOCK_FILE"
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
awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -q 'busybox_libs' \
    && ok "enter_ramfs stages the shared libraries busybox needs" \
    || bad "busybox is dynamically linked; staging it without its libs gives an unrunnable ramfs"
# ...and the same list is what the RAM check counts, or a uClibc camera is
# measured for a stage it is not given.
awk '/^ramfs_stage_kb\(\)/,/^}/' "$SRC" | grep -q 'busybox_libs' \
    && ok "ramfs_stage_kb counts what enter_ramfs stages" \
    || bad "ramfs_stage_kb must count busybox_libs, the list enter_ramfs copies"
# The probe that the staged root runs has to come before anything moves: after
# the pivot a missing loader is an exec with no root left to return to (#2605).
c=$(awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -n 'chroot "\$RAM_ROOT" /bin/busybox true' | head -1 | cut -d: -f1)
m=$(awk '/^enter_ramfs\(\)/,/^}/' "$SRC" | grep -n 'mount -o move /dev' | head -1 | cut -d: -f1)
[ -n "$c" ] && [ -n "$m" ] && [ "$c" -lt "$m" ] \
    && ok "enter_ramfs proves the staged root runs before it moves a mount" \
    || bad "enter_ramfs must chroot-probe the staged root before the mount moves (probe=${c:-none} move=${m:-none})"
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
# write_image and ubi_update are the places a write is not guarded on its own
# line: the write IS their return status, and their callers carry the `||`.
wi_lines=$(awk '/^(write_image|ubi_update)\(\)/,/^}/ { print NR }' "$SRC")
if grep -n 'set_progress \(flash\|ubiupdatevol\)' "$SRC" | grep -v '||' |
    grep -qvE "^($(echo $wi_lines | tr ' ' '|')):"; then
    bad "an unguarded set_progress write: $(grep -n 'set_progress \(flash\|ubiupdatevol\)' "$SRC" | grep -v '||')"
elif grep -n 'write_image "' "$SRC" | grep -qv '||'; then
    bad "an unguarded write_image call: $(grep -n 'write_image "' "$SRC" | grep -v '||')"
elif grep -n 'ubi_update "' "$SRC" | grep -v '||' | grep -qvE "^($(echo $wi_lines | tr ' ' '|')):"; then
    bad "an unguarded ubi_update call: $(grep -n 'ubi_update "' "$SRC" | grep -v '||')"
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
for fn in do_update_rootfs do_update_firmware_split do_wipe_overlay; do
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
# check_platform runs after the ramfs pivot, where there is no /etc/os-release:
# a read of the running build there comes back empty, and an empty platform is
# "not a device profile", so the check passed silently -- which is what the
# first on-camera run of it did (#2484). The running platform has to be taken
# before the pivot and carried across it.
cp=$(sed -n '/^check_platform()/,/^}/p' "$SRC")
if printf '%s\n' "$cp" | grep -qE 'get_system_platform *\)|get_system_platform *""|/etc/os-release'; then
    bad "check_platform reads the running os-release itself; it runs in the ramfs"
elif ! grep -qE '^[[:space:]]*export .*\bsystem_platform\b' "$SRC"; then
    bad "system_platform is not exported across the ramfs pivot"
else
    ok "check_platform uses the pre-pivot platform, and it crosses the pivot"
fi

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
f=$(printf '%s\n' "$body" | grep -nE 'write_image|flashcp'  | head -1 | cut -d: -f1)
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
