#!/bin/bash
# Regression tests for init's decision to erase rootfs_data.
#
# init formats rootfs_data when jffs2 will not mount it, or mounts it and
# complains about blocks that hold no jffs2. That is right for a partition
# that is not ours -- a vendor's leftovers on a camera just converted -- and
# was wrong for our own overlay: a kernel built without an option the overlay
# was written with refuses the whole filesystem (an xattr node is
# JFFS2_FEATURE_INCOMPAT), and the format then took every setting and the root
# password. Measured on an XM510 moved from a kernel with jffs2 xattrs to one
# without.
#
# overlay_is_ours() is what now tells the two apart, from names jffs2 keeps
# uncompressed in its directory entries. Part 1 runs the real function against
# synthetic partitions; part 2 pins that the format is still gated on it.
#
# Part 3 runs the settings carry-over a split-NOR upgrade leaves behind.
#
# Pure shell, no root, no QEMU.

set -u

# Overridable so the suite can be pointed at the comment-stripped build output.
SRC=${SRC:-general/overlay/init}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

[ -f "$SRC" ] || { echo "FAIL cannot find $SRC — run me from the repo root"; exit 1; }

SB=$(mktemp -d)
trap 'rm -rf "$SB"' EXIT

# The function alone, out of the script: init itself runs as PID 1 and mounts
# things at the top level, so it cannot be sourced.
{ grep '^OVERLAY_MARK=' "$SRC"; sed -n '/^overlay_is_ours() {/,/^}/p' "$SRC"; } > "$SB/fn.sh"
[ -s "$SB/fn.sh" ] || { echo "FAIL overlay_is_ours() not found in $SRC"; exit 1; }
# shellcheck disable=SC1091
. "$SB/fn.sh"

# A partition of erased flash (0xff) with $1 written at an offset, the way a
# jffs2 directory-entry node carries a name: binary header bytes, then the
# name, then more binary.
partition() {
	local f=$SB/$2
	head -c 65536 /dev/zero | tr '\000' '\377' > "$f"
	if [ -n "$1" ]; then
		printf '\x85\x19\x01\xe0\x00\x00\x00\x40\x12\x34%s\x00\x00\x56' "$1" |
			dd of="$f" bs=1 seek=4096 conv=notrunc 2>/dev/null
	fi
	echo "$f"
}

expect() {
	local want=$1 f=$2 desc=$3
	if overlay_is_ours "$f"; then got=ours; else got=not-ours; fi
	[ "$want" = "$got" ] && ok "$desc" || bad "$desc -- want $want, got $got"
}

# ---------------------------------------------------------------------------
# Part 1: the decision
expect ours     "$(partition .openipc-overlay mark)"  "an overlay init has marked is ours, before any setting is in it"
expect ours     "$(partition eula-accepted claim)"    "a claimed camera's overlay is ours"
expect ours     "$(partition majestic.yaml cfg)"      "an overlay holding a saved majestic setting is ours"
expect ours     "$(partition fake-hwclock.data hw)"   "an overlay fake-hwclock has saved to is ours"
expect not-ours "$(partition dropbear_ed25519_host_key db)" "dropbear's keys alone are not ours: vendor firmware ships dropbear too"
expect not-ours "$(partition '' blank)"               "erased flash is not ours (and formatting it costs nothing)"
expect not-ours "$(partition 'Config/SensorType.bat' vendor)" "a vendor's leftovers are not ours"
expect not-ours "$SB/does-not-exist"                  "an unreadable partition is not ours: the format decides, as before"

# ---------------------------------------------------------------------------
# Part 2: drift. The format must stay behind the check, and the branch that
# keeps an unreadable overlay must fall back to RAM rather than give up.
body=$(sed -n '/Magic bitmask/,/^elif \$is_nfs_root/p' "$SRC")
if echo "$body" | awk '/overlay_is_ours/ && !g {g=NR} /flash_eraseall/ && !e {e=NR} END {exit !(g && e && g < e)}'; then
	ok "flash_eraseall is reached only after overlay_is_ours"
else
	bad "flash_eraseall is no longer gated on overlay_is_ours in $SRC"
fi
if grep -q '> "/overlay/$OVERLAY_MARK"' "$SRC"; then
	ok "init marks the overlay as ours once it is mounted"
else
	bad "init no longer writes \$OVERLAY_MARK into a mounted overlay in $SRC"
fi
if echo "$body" | grep -q 'mount -t tmpfs tmpfs /overlay'; then
	ok "an overlay this kernel cannot mount falls back to tmpfs"
else
	bad "no tmpfs fallback for an unmountable overlay in $SRC"
fi

# ---------------------------------------------------------------------------
# Part 3: settings carried over by a split-NOR sysupgrade. The blob sits where
# rootfs_data now starts: "OPENIPC-OVL1", a 12-digit decimal length, a gzipped
# tar. The function runs for real; mount, umount and flash_eraseall are stubs
# that log, and /overlay and /mnt are moved into the sandbox.
sed -n '/^overlay_carry_in() {/,/^}/p' "$SRC" |
	sed -e "s| /overlay| $SB/ovl|g; s|/overlay/|$SB/ovl/|g; s| /mnt| $SB/mnt|g" > "$SB/carry.sh"
[ -s "$SB/carry.sh" ] || { echo "FAIL overlay_carry_in() not found in $SRC"; exit 1; }
mount() { echo "mount $*" >> "$SB/log"; mkdir -p "${!#}"; }
umount() { echo "umount $*" >> "$SB/log"; }
flash_eraseall() { echo "flash_eraseall $*" >> "$SB/log"; rm -rf "$SB/mnt"; }
sync() { :; }
# shellcheck disable=SC1091
. "$SB/carry.sh"
# blob <file> <tgz or empty>: the blob at the start of a 192 KiB partition of
# old data, so a length read wrong reads that.
blob() {
	head -c 196608 /dev/zero | tr '\000' '\132' > "$1"
	{ printf 'OPENIPC-OVL1%012d' "$(stat -c %s "${2:-/dev/null}" 2>/dev/null || echo 0)"
	  [ -n "${2:-}" ] && cat "$2"; } | dd of="$1" conv=notrunc 2>/dev/null
}
carry() { rm -rf "$SB/mnt" "$SB/ovl" "$SB/log"; : > "$SB/log"; OUT=$(overlay_carry_in "$1" "$SB/blk" 2>&1); }

mkdir -p "$SB/settings/etc"; echo kept > "$SB/settings/etc/marker"
# Past one erase block, so the length decides how much of the partition is read.
head -c 70000 /dev/urandom > "$SB/settings/etc/big"
tar -C "$SB/settings" -czf "$SB/settings.tgz" .
blob "$SB/part" "$SB/settings.tgz"
carry "$SB/part"
if [ "$(cat "$SB/mnt/etc/marker" 2>/dev/null)" = kept ] && cmp -s "$SB/mnt/etc/big" "$SB/settings/etc/big" &&
	grep -q "flash_eraseall -j $SB/part" "$SB/log" && echo "$OUT" | grep -q "Settings restored"; then
	ok "a carried-over blob is restored into a freshly erased rootfs_data"
else
	bad "carry-over restore, out='$OUT' log='$(cat "$SB/log")'"
fi
e=$(grep -n flash_eraseall "$SB/log" | cut -d: -f1); m=$(grep -n "mount -t jffs2" "$SB/log" | cut -d: -f1)
[ -n "$e" ] && [ -n "$m" ] && [ "$e" -lt "$m" ] \
	&& ok "...the erase comes before the jffs2 mount" \
	|| bad "carry-over must erase before it mounts, log='$(cat "$SB/log")'"

blob "$SB/part" ""
carry "$SB/part"
if grep -q "flash_eraseall -j $SB/part" "$SB/log" && ! grep -q "jffs2" "$SB/log" &&
	echo "$OUT" | grep -q "Clearing settings"; then
	ok "a blob of length 0 (sysupgrade -n) erases and restores nothing"
else
	bad "length-0 blob, out='$OUT' log='$(cat "$SB/log")'"
fi

head -c 196608 /dev/zero | tr '\000' '\377' > "$SB/part"
carry "$SB/part"
[ ! -s "$SB/log" ] && [ -z "$OUT" ] \
	&& ok "no blob: rootfs_data is left to the usual mount" \
	|| bad "without a blob nothing may be touched, log='$(cat "$SB/log")'"

echo
[ "$fail" -eq 0 ] && echo "All init overlay checks passed." || echo "$fail check(s) failed."
exit "$fail"
