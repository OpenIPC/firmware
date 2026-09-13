#!/bin/bash
# Regression tests for the guard that stops general/overlay/init formatting an
# overlay partition that runs off the end of the flash chip.
#
# #1998: SigmaStar sizes any NOR chip missing from its JEDEC table at 16MB. On a
# real 8MB camera the "-(rootfs_data)" partition then spans 8896KB, jffs2 cannot
# mount what is really an aliased mirror of the squashfs, and the format that
# follows erases past 0x800000 -- which a 3-byte-addressed chip wraps back to
# zero, taking the bootloader with it. The camera boots exactly once.
#
# The guard may only ever refuse on positive proof: a false positive costs a
# camera its persistent settings, so every case where the proof cannot be
# obtained must fall through and format as before. Both halves are asserted here.
#
# Pure shell, no root, no device. Runs in a second.

set -u

SRC=${SRC:-general/overlay/init}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

[ -f "$SRC" ] || { echo "FAIL cannot find $SRC -- run me from the repo root"; exit 1; }

# The device is busybox ash, so test against that and nothing else -- see the
# header of test_shell_parse.sh for why dash is not a stand-in.
BUSYBOX=$(command -v busybox 2>/dev/null || true)
if [ -z "$BUSYBOX" ]; then
	if [ "${STRICT:-0}" != "0" ]; then
		echo "FAIL busybox not found and STRICT=$STRICT -- refusing to report a"
		echo "     pass for a check that did not run. Install busybox-static."
		exit 1
	fi
	echo "note busybox not found, skipping."
	exit 0
fi

SB=$(mktemp -d)
trap 'rm -rf "$SB"' EXIT
mkdir -p "$SB/proc" "$SB/dev"

# Lift the two functions out of the real init and point them at the sandbox, so
# the code under test is the code that ships rather than a copy that can drift.
# Everything between the first function header and the closing brace of
# mtd_geometry is taken verbatim.
sed -n '/^overlay_wraps_past_end_of_flash()/,/^}/p;/^mtd_geometry()/,/^}/p' "$SRC" \
	| sed -e "s|/proc/mtd|$SB/proc/mtd|g" \
	      -e "s|/dev/mtd0|$SB/dev/mtd0|g" \
	      -e "s|\"/dev/|\"$SB/dev/|g" > "$SB/guard.sh"

for fn in overlay_wraps_past_end_of_flash mtd_geometry; do
	grep -q "^$fn()" "$SB/guard.sh" || { echo "FAIL could not lift $fn out of $SRC"; exit 1; }
done
grep -q "$SB/dev/mtd0" "$SB/guard.sh" || { echo "FAIL sandbox rewrite missed the mtd0 read"; exit 1; }
grep -q "$SB/dev/\$_chrdev" "$SB/guard.sh" || { echo "FAIL sandbox rewrite missed the partition read"; exit 1; }

# --- fixtures --------------------------------------------------------------

# A believable first sector: the point is only that it is not blank, so that an
# aliased read of it is distinguishable from erased flash.
make_bootloader() { head -c "$1" /dev/urandom > "$2"; }

blank() { tr '\0' '\377' < /dev/zero | head -c "$1"; }

# Build a whole chip as one file, then carve the partitions out of it exactly as
# mtdparts would -- with the wrap, if the chip is smaller than the table claims.
#
#   $1 real chip size   $2 believed chip size   $3 rootfs_data offset
build_chip() {
	real=$1; believed=$2; off=$3
	head -c "$real" /dev/urandom > "$SB/chip"
	head -c 262144 "$SB/chip" > "$SB/dev/mtd0"
	# What the driver presents as rootfs_data: the real tail of the chip, then --
	# if it believes the chip is bigger than it is -- the chip over again, because
	# a 3-byte SPI address at or past the real end wraps back to zero.
	need=$((believed - off))
	{
		dd if="$SB/chip" bs=512 skip=$((off / 512)) 2>/dev/null
		n=0
		while [ $((n * real)) -lt "$need" ]; do cat "$SB/chip"; n=$((n + 1)); done
	} 2>/dev/null | head -c "$need" > "$SB/dev/mtd4"
}

set_mtd() { cat > "$SB/proc/mtd"; }

# The partition table an 8MB OpenIPC image asks for. rootfs_data's size is the
# only thing that moves: it is whatever the driver thinks is left of the chip.
mtd_table() {
	printf 'dev:    size   erasesize  name\n'
	printf 'mtd0: 00040000 00010000 "boot"\n'
	printf 'mtd1: 00010000 00010000 "env"\n'
	printf 'mtd2: 00200000 00010000 "kernel"\n'
	printf 'mtd3: 00500000 00010000 "rootfs"\n'
	printf 'mtd4: %08x 00010000 "rootfs_data"\n' "$1"
}

# Run the lifted functions the way init calls them and report the verdict as
# the words a reader of this test cares about.
verdict() {
	"$BUSYBOX" ash -c ". $SB/guard.sh
		mtd_geometry || { echo no-geometry; exit 0; }
		if overlay_wraps_past_end_of_flash \"\$(basename $SB/dev/mtd4)\" \"\$mtdoffset\" \"\$mtdsize\"; then
			echo refuse
		else
			echo format
		fi"
}

geometry() {
	"$BUSYBOX" ash -c ". $SB/guard.sh
		mtd_geometry && printf '%s %s\n' \"\$mtdoffset\" \"\$mtdsize\""
}

OFF=$((0x750000))

# --- 1. the reported brick -------------------------------------------------
# 8MB chip, driver says 16MB, so rootfs_data is 8896KB and its tail is a mirror
# of the whole chip starting with the bootloader.
mtd_table $((0x8b0000)) | set_mtd
build_chip $((0x800000)) $((0x1000000)) "$OFF"
got=$(verdict)
[ "$got" = refuse ] && ok "8MB chip sized 16MB: refuses to format (#1998)" \
	|| bad "8MB chip sized 16MB: expected refuse, got '$got'"

# --- 2. a healthy 8MB camera ----------------------------------------------
# rootfs_data is the real 704KB tail. There is no plausible smaller chip end
# inside the partition, so the guard must not even look.
mtd_table $((0x0b0000)) | set_mtd
build_chip $((0x800000)) $((0x800000)) "$OFF"
got=$(verdict)
[ "$got" = format ] && ok "healthy 8MB camera: formats as before" \
	|| bad "healthy 8MB camera: expected format, got '$got'"

# --- 3. a real 16MB chip carrying the 8MB layout ---------------------------
# Same partition table as case 1 -- this is the geometry openipc.org hands out
# when a visitor picks the 8MB layout on a 16MB chip -- but nothing aliases.
mtd_table $((0x8b0000)) | set_mtd
build_chip $((0x1000000)) $((0x1000000)) "$OFF"
got=$(verdict)
[ "$got" = format ] && ok "real 16MB chip, 8MB layout: formats as before" \
	|| bad "real 16MB chip, 8MB layout: expected format, got '$got'"

# --- 4. blank first sector -------------------------------------------------
# An erased sector matches any other erased sector, so it proves nothing and
# must not be allowed to. Keep case 1's aliasing otherwise.
mtd_table $((0x8b0000)) | set_mtd
build_chip $((0x800000)) $((0x1000000)) "$OFF"
blank 262144 > "$SB/dev/mtd0"
blank $((0x8b0000)) > "$SB/dev/mtd4"
got=$(verdict)
[ "$got" = format ] && ok "blank flash proves nothing: formats as before" \
	|| bad "blank flash: expected format, got '$got'"

# --- 5. no mtd0 to read ----------------------------------------------------
mtd_table $((0x8b0000)) | set_mtd
build_chip $((0x800000)) $((0x1000000)) "$OFF"
rm -f "$SB/dev/mtd0"
got=$(verdict)
[ "$got" = format ] && ok "unreadable mtd0: falls through and formats" \
	|| bad "unreadable mtd0: expected format, got '$got'"

# --- 6. no rootfs_data at all ----------------------------------------------
{ printf 'dev:    size   erasesize  name\n'; printf 'mtd0: 00040000 00010000 "boot"\n'; } | set_mtd
got=$(verdict)
[ "$got" = no-geometry ] && ok "no rootfs_data partition: guard stands down" \
	|| bad "no rootfs_data partition: expected no-geometry, got '$got'"

# --- 7. the octal trap -----------------------------------------------------
# /proc/mtd pads sizes to eight hex digits. Read without an 0x prefix, 00040000
# is octal 16384 rather than 262144, and every offset after it is wrong.
mtd_table $((0x8b0000)) | set_mtd
got=$(geometry)
[ "$got" = "$((0x750000)) $((0x8b0000))" ] \
	&& ok "hex sizes with leading zeros are not read as octal" \
	|| bad "geometry: expected '$((0x750000)) $((0x8b0000))', got '$got'"

echo
if [ "$fail" -ne 0 ]; then
	echo "$fail check(s) failed."
	exit 1
fi
echo "All overlay format guard checks passed."
