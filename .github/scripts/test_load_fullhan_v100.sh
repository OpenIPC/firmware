#!/bin/sh
# load_fullhan (FH885x V100, FH8632): where it puts the media pool and the ARC.
#
# The ARC's image runs at the top of a 128 MB DDR, so there rtvbus's ring and
# firmware take its last 2 MB, and vmm.ko's mmz runs from the end of mem= up
# to them. A 64 MB chip keeps the fixed layout it always had.
# The same script runs on 64 MB FH8852 and FH8632 chips and 128 MB FH8856
# ones, booted with OpenIPC's mem=29M or a vendor environment's mem=36M, so
# each of those is a case below -- and the 64 MB, mem=29M case must stay
# exactly the layout it always was.
#
# Runs the real script under busybox sh, as on the camera, from a wrapper that
# defines modprobe, logger, devmem, usleep and the GPIO writes away as shell
# functions -- busybox sh runs its own applets ahead of anything on PATH, but
# not ahead of functions. /proc/cmdline and /proc/driver/chip come from
# LOAD_FULLHAN_CMDLINE and LOAD_FULLHAN_CHIP; SCRIPT_UNDER_TEST runs another
# copy of the script, such as the comment-stripped one that ships.

set -eu

if ! command -v busybox >/dev/null 2>&1; then
	echo "FAIL busybox is needed: it is the shell the camera runs"
	exit 1
fi

script=${SCRIPT_UNDER_TEST:-$(pwd)/general/package/fullhan-osdrv-fh8852v100/files/script/load_fullhan}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

cat >"$work/wrapper" <<'EOF'
modprobe() { echo "modprobe $*" >>"$CALLS"; }
logger() { echo "logger $*" >>"$CALLS"; }
# the ARC is up as soon as it is asked, at whatever address it is asked about
devmem() { echo "devmem $*" >>"$CALLS"; echo 0x00000001; }
usleep() { :; }
set_gpio() { :; }
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

# layout POOL ARC-RING WHAT
layout() {
	fw=$(printf '0x%x' $(($2 + 0x100000)))
	if grep -qxF "modprobe vmm mmz=anonymous,0,$1 anony=1" "$work/calls"; then
		ok "$3: pool $1"
	else
		bad "$3 -- want pool $1, got: $(grep vmm "$work/calls" | tr '\n' ';')"
	fi
	if grep -qxF "modprobe rtvbus ra=$2 rs=65536 fn=rtthread_arc_FH8852.bin fa=$fw" "$work/calls"; then
		ok "$3: ARC ring $2, firmware $fw"
	else
		bad "$3 -- want ARC ring $2 firmware $fw, got: $(grep rtvbus "$work/calls" | tr '\n' ';')"
	fi
	if grep -qxF "devmem $(printf '0x%x' $(($2 + 0x7c))) 32" "$work/calls"; then :; else
		bad "$3 -- the ARC was not polled in its own ring: $(grep devmem "$work/calls" | tr '\n' ';')"
	fi
	if [ "$rc" = 0 ] && grep -q '^modprobe vbus_ac' "$work/calls"; then :; else
		bad "$3 -- the script did not run to its end (exit $rc): $(tail -3 "$work/out" | tr '\n' ';')"
	fi
}
logged() {
	if grep -q "^logger .*$1" "$work/calls"; then ok "$2"; else bad "$2 -- no log matching '$1'"; fi
}
quiet() {
	if grep -q '^logger' "$work/calls"; then bad "$1 -- logged: $(grep '^logger' "$work/calls" | tr '\n' ';')"; else ok "$1"; fi
}

# as OpenIPC's 3.0.8 kernel prints it on an FH8856 (pkg_id 0xd) and FH8852 (0xc)
chip() { printf 'chip_name\t: %s\nddr_size\t: %sMbit\nplat_id\t\t: 0x17092901\npkg_id\t\t: 0x%s\n' "$1" "$2" "$3"; }
boot='console=ttyS0,115200 panic=20 root=/dev/mtdblock3 rootfstype=squashfs init=/init mtdparts=spi_flash:256k(boot)'

echo "=== OpenIPC's mem=29M ==="

run "mem=29M $boot" "$(chip FH8852 512 c)"
layout "0xA1D00000,33M" 0xa3e00000 "64 MB: the layout it always had"
quiet "... and says nothing"

run "mem=29M $boot" "$(chip FH8856 1024 d)"
layout "0xA1D00000,97M" 0xa7e00000 "128 MB: the ARC at the top, where its image runs"
quiet "... and says nothing"

run "$boot mem=29M" "$(chip FH8856 1024 d)"
layout "0xA1D00000,97M" 0xa7e00000 "mem= as the last argument"

echo "=== a vendor environment left behind, mem=36M ==="

run "mem=36M $boot" "$(chip FH8856 1024 d)"
layout "0xA2400000,90M" 0xa7e00000 "128 MB: the vendor's own layout"

run "mem=36M@0xA0000000 $boot" "$(chip FH8856 1024 d)"
layout "0xA2400000,90M" 0xa7e00000 "mem=36M at the DDR's own address: the same"

run "mem=36M $boot" "$(chip FH8852 512 c)"
layout "0xA1D00000,33M" 0xa3e00000 "64 MB: the layout it always had, whatever mem= says"
quiet "... and says nothing"

run "mem=96M $boot" "$(chip FH8856 1024 d)"
layout "0xA6000000,30M" 0xa7e00000 "128 MB, a large mem=: the pool still ends below the ARC"
logged "the ISP may not start" "... and says the pool is small"

echo "=== nothing to go on: the fixed 64 MB layout ==="

run "$boot" "$(chip FH8856 1024 d)"
layout "0xA1D00000,33M" 0xa3e00000 "no mem= on the command line"
logged "fixed 64 MB layout kept" "... and says so"

run "mem=29M $boot" ""
layout "0xA1D00000,33M" 0xa3e00000 "no /proc/driver/chip"
logged "fixed 64 MB layout kept" "... and says so"

run "mem=29M $boot" "$(chip FH8856 2048 d)"
layout "0xA1D00000,33M" 0xa3e00000 "a DDR size no V100 chip has"

run "mem=120M $boot" "$(chip FH8856 1024 d)"
layout "0xA1D00000,33M" 0xa3e00000 "mem= leaving no room for a pool"
logged "fixed 64 MB layout kept" "... and says so"

run "mem=33554432 $boot" "$(chip FH8856 1024 d)"
layout "0xA1D00000,33M" 0xa3e00000 "mem= not in megabytes"

run "mem=29M mem=16M@0xA4000000 $boot" "$(chip FH8856 1024 d)"
layout "0xA1D00000,33M" 0xa3e00000 "two mem= (a second bank)"

run "mymem=64M $boot" "$(chip FH8856 1024 d)"
layout "0xA1D00000,33M" 0xa3e00000 "an argument that only ends in mem="

if [ "$fail" -ne 0 ]; then
	echo "$fail check(s) failed"
	exit 1
fi
echo "all load_fullhan (V100) checks passed"
