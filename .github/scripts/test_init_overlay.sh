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
sed -n '/^overlay_carry_in() {/,/^}/p; /^carry_report() {/,/^}/p' "$SRC" |
	sed -e "s| /overlay| $SB/ovl|g; s|/overlay/|$SB/ovl/|g; s| /mnt| $SB/mnt|g" > "$SB/carry.sh"
[ -s "$SB/carry.sh" ] || { echo "FAIL overlay_carry_in() not found in $SRC"; exit 1; }
mount() {
	echo "mount $*" >> "$SB/log"
	[ "$2" = tmpfs ] && [ -n "${TMPFS_FAILS:-}" ] && return 1
	mkdir -p "${!#}"
}
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
carry() {
	rm -rf "$SB/mnt" "$SB/ovl" "$SB/log"; : > "$SB/log"
	OUT=$(overlay_carry_in "$1" "$SB/blk" 2>&1); RC=$?
}

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

# A damaged archive: reported, and nothing erased -- the usual mount decides.
blob "$SB/part" "$SB/settings.tgz"
printf 'XXXX' | dd of="$SB/part" bs=1 seek=40 conv=notrunc 2>/dev/null
carry "$SB/part"
if [ "$RC" -eq 0 ] && ! grep -q flash_eraseall "$SB/log" && echo "$OUT" | grep -q "damaged"; then
	ok "a damaged carry-over is reported and nothing is erased"
else
	bad "damaged carry-over, rc=$RC out='$OUT' log='$(cat "$SB/log")'"
fi

# A length past what the partition holds: kept for the next boot (rc 1).
blob "$SB/part" "$SB/settings.tgz"
printf 'OPENIPC-OVL1%012d' 999999 | dd of="$SB/part" conv=notrunc 2>/dev/null
carry "$SB/part"
if [ "$RC" -eq 1 ] && ! grep -q flash_eraseall "$SB/log"; then
	ok "a short read keeps the blob for the next boot, nothing erased"
else
	bad "short read, rc=$RC out='$OUT' log='$(cat "$SB/log")'"
fi

# No RAM for the archive: kept for the next boot too.
blob "$SB/part" "$SB/settings.tgz"
TMPFS_FAILS=1 carry "$SB/part"
if [ "$RC" -eq 1 ] && ! grep -q flash_eraseall "$SB/log"; then
	ok "no tmpfs: the blob is kept for the next boot, nothing erased"
else
	bad "tmpfs failure, rc=$RC out='$OUT' log='$(cat "$SB/log")'"
fi

head -c 196608 /dev/zero | tr '\000' '\377' > "$SB/part"
carry "$SB/part"
[ ! -s "$SB/log" ] && [ -z "$OUT" ] \
	&& ok "no blob: rootfs_data is left to the usual mount" \
	|| bad "without a blob nothing may be touched, log='$(cat "$SB/log")'"

# Drift: the carry-over runs only where the alias guard allows, and a blob it
# kept sends init to a RAM overlay rather than the jffs2 path that formats.
call=$(sed -n '/aliased=false/,/elif ! mount -t jffs2/p' "$SRC")
if echo "$call" | grep -q 'flash_aliased' && echo "$call" | grep -q '! \$aliased' &&
	echo "$call" | awk '/flash_aliased/ && !a {a=NR} /overlay_carry_in/ && !c {c=NR} END {exit !(a && c && a < c)}'; then
	ok "the carry-over runs after the alias check, and only on flash that is not aliased"
else
	bad "overlay_carry_in must come after flash_aliased and be skipped on aliased flash in $SRC"
fi
if echo "$call" | grep -q 'carried" = 1' && echo "$call" | grep -q 'mount -t tmpfs tmpfs /overlay'; then
	ok "a kept blob boots on a RAM overlay"
else
	bad "a carry-over kept for the next boot must not reach the jffs2 mount in $SRC"
fi

# ---------------------------------------------------------------------------
# Part 4: flash_aliased() on a kernel without partition offsets in sysfs (3.x).
#
# The partitions' positions then come from their order. A 16 MB layout on an
# 8 MB chip that wraps past its end: the block at 8 MB reads as block 0, so the
# answer is "aliased". The split layout lists "firmware" before the kernel,
# rootfs and rootfs_data it holds; taken as one more partition in a row it
# pushed them all past the chip, and the check never looked at 8 MB.
sed -n '/^flash_aliased() {/,/^}/p' "$SRC" |
	sed -e "s|/proc/mtd|$SB/proc/mtd|g" -e "s|/sys/class/mtd|$SB/sys/class/mtd|g" \
		-e "s|if=/dev/|if=$SB/dev/|g" > "$SB/alias.sh"
[ -s "$SB/alias.sh" ] || bad "flash_aliased() not found in $SRC"
# shellcheck disable=SC1091
. "$SB/alias.sh"

# mtd_layout "name:offset:size"...: /proc/mtd and one file per partition, the
# chip's 8 MB repeated across the 16 MB, block 0 marked.
mtd_layout() {
	local i=0 spec name off size
	rm -rf "$SB/proc" "$SB/dev" "$SB/sys"; mkdir -p "$SB/proc" "$SB/dev" "$SB/sys/class/mtd"
	head -c 65536 /dev/zero | tr '\000' 'B' > "$SB/block0"
	printf 'dev:    size   erasesize  name\n' > "$SB/proc/mtd"
	for spec in "$@"; do
		name=${spec%%:*}; spec=${spec#*:}; off=$((${spec%%:*})); size=$((${spec#*:}))
		printf 'mtd%d: %08x 00010000 "%s"\n' "$i" "$size" "$name" >> "$SB/proc/mtd"
		truncate -s "$size" "$SB/dev/mtd$i"
		# the chip's block 0 wherever this partition covers 0 or 8 MB
		for at in 0 $((0x800000)); do
			[ "$at" -ge "$off" ] && [ "$at" -lt $((off + size)) ] &&
				dd if="$SB/block0" of="$SB/dev/mtd$i" bs=65536 seek=$(((at - off) / 65536)) conv=notrunc 2>/dev/null
		done
		i=$((i + 1))
	done
}
M16=0x1000000
mtd_layout boot:0:0x40000 env:0x40000:0x10000 firmware:0x50000:$((M16 - 0x50000)) \
	kernel:0x50000:0x1e0000 rootfs:0x230000:0x280000 rootfs_data:0x4b0000:$((M16 - 0x4b0000))
if flash_aliased; then
	ok "split layout, no sysfs offsets: a 16 MB layout on a mirrored 8 MB chip is found aliased"
else
	bad "split layout without sysfs offsets: the mirror at 8 MB must be found"
fi
mtd_layout boot:0:0x40000 env:0x40000:0x10000 kernel:0x50000:0x200000 \
	rootfs:0x250000:0x500000 firmware:0x50000:0x700000 rootfs_data:0x750000:$((M16 - 0x750000))
if flash_aliased; then
	ok "fixed slots, no sysfs offsets: the same mirror is found past the firmware alias"
else
	bad "fixed slots without sysfs offsets: the mirror at 8 MB must be found"
fi
# Not mirrored: block 0 only at 0.
mtd_layout boot:0:0x40000 env:0x40000:0x10000 firmware:0x50000:$((M16 - 0x50000)) \
	kernel:0x50000:0x1e0000 rootfs:0x230000:0x280000 rootfs_data:0x4b0000:$((M16 - 0x4b0000))
# firmware (mtd2) and rootfs_data (mtd5) both reach 8 MB: neither mirrors
head -c $((M16 - 0x50000)) /dev/zero | tr '\000' '\377' > "$SB/dev/mtd2"
head -c $((M16 - 0x4b0000)) /dev/zero | tr '\000' '\377' > "$SB/dev/mtd5"
if ! flash_aliased; then
	ok "split layout, no sysfs offsets: a chip that does not wrap is not aliased"
else
	bad "split layout without sysfs offsets: an unwrapped chip must not be called aliased"
fi

echo
[ "$fail" -eq 0 ] && echo "All init overlay checks passed." || echo "$fail check(s) failed."
exit "$fail"
