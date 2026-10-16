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
sed -n '/^overlay_is_ours() {/,/^}/p' "$SRC" > "$SB/fn.sh"
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
expect ours     "$(partition fake-hwclock.data hw)"   "an overlay fake-hwclock has saved to is ours"
expect ours     "$(partition dropbear_ed25519_host_key db)" "an overlay holding dropbear's host keys is ours"
expect not-ours "$(partition '' blank)"               "erased flash is not ours (and formatting it costs nothing)"
expect not-ours "$(partition 'Config/SensorType.bat' vendor)" "a vendor's leftovers are not ours"
expect not-ours "$SB/does-not-exist"                  "an unreadable partition is not ours: the format decides, as before"

# ---------------------------------------------------------------------------
# Part 2: drift. The format must stay behind the check, and the branch that
# keeps an unreadable overlay must fall back to RAM rather than give up.
body=$(sed -n '/Magic bitmask/,/^elif \$is_nfs_root/p' "$SRC")
if echo "$body" | awk '/overlay_is_ours/ {g=NR} /flash_eraseall/ {e=NR} END {exit !(g && e && g < e)}'; then
	ok "flash_eraseall is reached only after overlay_is_ours"
else
	bad "flash_eraseall is no longer gated on overlay_is_ours in $SRC"
fi
if echo "$body" | grep -q 'mount -t tmpfs tmpfs /overlay'; then
	ok "an overlay this kernel cannot mount falls back to tmpfs"
else
	bad "no tmpfs fallback for an unmountable overlay in $SRC"
fi

echo
[ "$fail" -eq 0 ] && echo "All init overlay checks passed." || echo "$fail check(s) failed."
exit "$fail"
