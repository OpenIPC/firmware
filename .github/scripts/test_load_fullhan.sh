#!/bin/sh
# load_fullhan (FH885xV200/V210): where it puts the media pool.
#
# vmm.ko's mmz has to start where the kernel's memory (mem=) ends and may run
# to the end of the DDR. Every V200/V210 camera runs this script, on 64 MB and
# 128 MB chips, booted by OpenIPC's U-Boot (mem=32M) or with a vendor
# environment left behind (mem=36M), so each of those is a case below -- and
# the 64 MB, mem=32M case must stay exactly the layout it always was.
#
# Runs the real script under busybox sh, as on the camera, from a wrapper that
# defines insmod, logger and the GPIO writes away as shell functions -- busybox
# sh runs its own applets ahead of anything on PATH, but not ahead of
# functions. /proc/cmdline and /proc/driver/chip come from
# LOAD_FULLHAN_CMDLINE and LOAD_FULLHAN_CHIP; SCRIPT_UNDER_TEST runs another copy
# of the script, such as the comment-stripped one that ships.

set -eu

if ! command -v busybox >/dev/null 2>&1; then
	echo "FAIL busybox is needed: it is the shell the camera runs"
	exit 1
fi

script=${SCRIPT_UNDER_TEST:-$(pwd)/general/package/fullhan-osdrv-fh8852v200/files/script/load_fullhan}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

cat >"$work/wrapper" <<'EOF'
insmod() { echo "insmod $*" >>"$CALLS"; }
logger() { echo "logger $*" >>"$CALLS"; }
set_gpio() { :; }
cd() { :; }
# the script defines set_gpio itself; run it with that definition replaced
sed 's/^set_gpio()$/real_set_gpio()/' "$SCRIPT" >"$WORK/script"
. "$WORK/script"
EOF

# run CMDLINE CHIP-FILE-CONTENT
run() {
	printf '%s\n' "$1" >"$work/cmdline"
	if [ -n "$2" ]; then printf '%b' "$2" >"$work/chip"; else rm -f "$work/chip"; fi
	: >"$work/calls"
	rc=0
	(SCRIPT="$script" CALLS="$work/calls" WORK="$work" \
		LOAD_FULLHAN_CMDLINE="$work/cmdline" LOAD_FULLHAN_CHIP="$work/chip" \
		busybox sh "$work/wrapper" >"$work/out" 2>&1) || rc=$?
}

pool() {
	if grep -qxF "insmod vmm.ko mmz=anonymous,0,$1 anony=1" "$work/calls"; then
		ok "$2"
	else
		bad "$2 -- want $1, got: $(grep vmm "$work/calls" | tr '\n' ';')"
	fi
	if [ "$rc" = 0 ] && grep -q '^insmod jpeg.ko' "$work/calls"; then :; else
		bad "$2 -- the script did not run to its end (exit $rc): $(tail -3 "$work/out" | tr '\n' ';')"
	fi
}
logged() {
	if grep -q "^logger .*$1" "$work/calls"; then ok "$2"; else bad "$2 -- no log matching '$1'"; fi
}
quiet() {
	if grep -q '^logger' "$work/calls"; then bad "$1 -- logged: $(grep '^logger' "$work/calls" | tr '\n' ';')"; else ok "$1"; fi
}

chip() { printf 'chip_name\t: %s\nddr_size\t: %sMbit\nplat_id\t\t: 0x19112201\npkg_id\t\t: 0x7850b\n' "$1" "$2"; }
boot='console=ttyS0,115200 panic=20 root=/dev/mtdblock3 rootfstype=squashfs init=/init mtdparts=sfc:256k(boot)'

echo "=== OpenIPC U-Boot, mem=32M ==="

run "mem=32M $boot" "$(chip FH8852V200 512)"
pool "0xA2000000,32M" "64 MB: the layout it always had"
quiet "... and says nothing"

run "mem=32M $boot" "$(chip FH8856V200 1024)"
pool "0xA2000000,64M" "128 MB: 64M, the largest pool seen streaming there"
quiet "... and says nothing"

run "$boot mem=32M" "$(chip FH8856V200 1024)"
pool "0xA2000000,64M" "mem= as the last argument"

echo "=== a vendor environment left behind, mem=36M ==="

run "mem=36M $boot" "$(chip FH8856V200 1024)"
pool "0xA2400000,64M" "128 MB: the pool starts above the kernel, not 4 MB inside it"

run "mem=36M@0 $boot" "$(chip FH8856V200 1024)"
pool "0xA2400000,64M" "mem=36M@0: the same"

run "mem=36M@0xA0000000 $boot" "$(chip FH8856V200 1024)"
pool "0xA2400000,64M" "mem=36M at the DDR's own address: the same"

run "mem=36M $boot" "$(chip FH8852V200 512)"
pool "0xA2400000,28M" "64 MB: the pool starts above the kernel"
logged "the ISP may not start" "... and says the pool is small"

echo "=== nothing to go on: the fixed layout ==="

run "$boot" "$(chip FH8856V200 1024)"
pool "0xA2000000,32M" "no mem= on the command line"
logged "media pool left at" "... and says so"

run "mem=32M $boot" ""
pool "0xA2000000,32M" "no /proc/driver/chip"
logged "media pool left at" "... and says so"

run "mem=32M $boot" "$(chip UNKNOWN 0)"
pool "0xA2000000,32M" "an unknown chip (ddr_size 0)"

run "mem=32M $boot" "$(chip FH8856V200 768)"
pool "0xA2000000,32M" "a DDR size no such chip has"

run "mem=128M $boot" "$(chip FH8856V200 1024)"
pool "0xA2000000,32M" "mem= covering the whole DDR"

run "mem=32m $boot" "$(chip FH8856V200 1024)"
pool "0xA2000000,64M" "mem= with a lower-case m, as the kernel takes it"

run "mem=33554432 $boot" "$(chip FH8856V200 1024)"
pool "0xA2000000,32M" "mem= not in megabytes"

run "mem=36M@0x80000000 $boot" "$(chip FH8856V200 1024)"
pool "0xA2000000,32M" "mem= somewhere other than the DDR's start"
logged "media pool left at" "... and says so"

run "mem=32M mem=16M@0xA4000000 $boot" "$(chip FH8856V200 1024)"
pool "0xA2000000,32M" "two mem= (a second bank)"

run "mymem=64M $boot" "$(chip FH8856V200 1024)"
pool "0xA2000000,32M" "an argument that only ends in mem="

if [ "$fail" -ne 0 ]; then
	echo "$fail check(s) failed"
	exit 1
fi
echo "all load_fullhan checks passed"
