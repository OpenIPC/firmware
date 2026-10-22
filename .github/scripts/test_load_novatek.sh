#!/bin/sh
# load_novatek (NT98562/NT98566): what it does to the U-Boot environment.
#
# The vendor U-Boot lays memory out from osmem and the kernel takes mem= from
# bootargs; load_novatek keeps the two from disagreeing (check_mem, #2657) and
# moves a 128 MB board from the old osmem=32M to 48M once (set_default_osmem,
# #2107). Both write the environment of a camera nobody is watching, so every
# case that must NOT write is tested as carefully as the ones that must.
#
# Runs the real script the way S70vendor does at boot, under busybox sh as on
# the camera, from a wrapper that defines insmod, devmem, fw_printenv,
# fw_setenv, ipcinfo and logger as shell functions -- busybox sh runs its own
# applets ahead of anything on PATH, but not ahead of functions -- so every
# module load and environment write is recorded. The device tree and the
# kernel command line come from LOAD_NOVATEK_DT and LOAD_NOVATEK_CMDLINE.

set -eu

if ! command -v busybox >/dev/null 2>&1; then
	echo "FAIL busybox is needed: it is the shell the camera runs"
	exit 1
fi

script=$(pwd)/general/package/novatek-osdrv-nt9856x/files/script/load_novatek
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

cat >"$work/wrapper" <<'EOF'
insmod() { echo "insmod $1" >>"$CALLS"; }
devmem() { :; }
logger() { :; }
ipcinfo() { echo sc501ai; }
fw_printenv() { [ "$1" = -n ] && shift; sed -n "s/^$1=//p" "$ENV_FILE" | grep . || return 1; }
fw_setenv() {
	if [ "$1" = --script ]; then
		echo "fw_setenv --script" >>"$CALLS"
		sed 's/^/batch /' "$2" >>"$CALLS"
	else
		echo "fw_setenv $*" >>"$CALLS"
	fi
}
export SENSOR=sc501ai
set -- -i
. "$SCRIPT"
EOF

# A one-address, one-size reg property: 8 big-endian bytes, base 0.
reg() {
	mkdir -p "$(dirname "$1")"
	size=$(( $2 << 20 ))
	printf "\\000\\000\\000\\000\\$(printf %03o $(( (size >> 24) & 255 )))\\$(printf %03o $(( (size >> 16) & 255 )))\\$(printf %03o $(( (size >> 8) & 255 )))\\$(printf %03o $(( size & 255 )))" >"$1"
}

# run DRAM_MB DT_MEM_MB CMDLINE then the environment, one key=value per
# argument; every call is left in $work/calls
run() {
	rm -rf "$work/dt"
	reg "$work/dt/nvt_memory_cfg/dram/reg" "$1"
	reg "$work/dt/memory/reg" "$2"
	printf '%s\n' "$3" >"$work/cmdline"
	shift 3
	: >"$work/env"
	for kv in "$@"; do
		printf '%s\n' "$kv" >>"$work/env"
	done
	: >"$work/calls"
	rc=0
	(cd "$work" && SCRIPT="$script" CALLS="$work/calls" ENV_FILE="$work/env" \
		LOAD_NOVATEK_DT="$work/dt" LOAD_NOVATEK_CMDLINE="$work/cmdline" \
		busybox sh "$work/wrapper" >"$work/out" 2>&1) || rc=$?
}

# The script ran to its end (0), or stopped where it means to (check_mem's
# exit 1) -- so a no-write case cannot pass because the script died first.
exited() {
	if [ "$rc" = "$1" ]; then ok "$2"; else bad "$2 -- exit $rc, want $1: $(tail -3 "$work/out" | tr '\n' ';')"; fi
}

has() {
	if grep -qF -- "$1" "$work/calls"; then ok "$2"; else bad "$2 -- no '$1' in: $(tr '\n' ';' <"$work/calls")"; fi
}
lacks() {
	if grep -qF -- "$1" "$work/calls"; then bad "$2 -- found '$1' in: $(tr '\n' ';' <"$work/calls")"; else ok "$2"; fi
}
loaded() {
	if grep -q '^insmod nvt_sen_sc501ai.ko' "$work/calls"; then ok "$1"; else bad "$1 -- HDAL and the sensor were not loaded"; fi
}

lit='earlyprintk console=ttyS0,115200 mem=32M panic=20 root=/dev/mtdblock3 rootfstype=squashfs'
ph='earlyprintk console=ttyS0,115200 mem=${osmem} panic=20 root=/dev/mtdblock3 rootfstype=squashfs'

echo "=== the 48M default on 128 MB boards ==="

run 128 32 "$lit" "osmem=32M" "bootargs=$lit"
has "fw_setenv --script" "osmem=32M on 128 MB: one batch write"
has "batch osmem 48M" "... osmem goes to 48M"
has "batch bootargs earlyprintk console=ttyS0,115200 mem=48M panic=20" "... and mem= with it, the rest of bootargs untouched"
has "batch osmem_default 48M" "... marked done"
exited 0 "... and the script runs to its end"
lacks "fw_setenv osmem" "... osmem is never written on its own, apart from mem="
loaded "... and this boot still loads HDAL at 32M"

run 128 32 "$lit" "bootargs=$lit"
has "batch osmem 48M" "osmem unset (U-Boot's 32M default): moved too"

run 128 32 "$lit" "osmem=32M" "bootargs=$ph"
has "batch osmem 48M" "bootargs with the \${osmem} placeholder: osmem moves"
lacks "batch bootargs" "... and bootargs is left alone, the placeholder carries it"

run 128 48 "earlyprintk mem=48M panic=20" "osmem=48M" "osmem_default=48M" "bootargs=earlyprintk mem=48M panic=20"
lacks "fw_setenv" "the boot after: nothing written"
loaded "... and HDAL loads at 48M"

run 128 32 "$lit" "osmem=32M" "osmem_default=48M" "bootargs=$lit"
lacks "fw_setenv" "an owner who set 32M again after the move keeps it"
exited 0 "... and the script runs to its end"

run 128 54 "earlyprintk mem=54M" "osmem=54M" "bootargs=earlyprintk mem=54M"
lacks "fw_setenv" "an osmem of the owner's own (54M): left alone"

run 64 32 "$lit" "osmem=32M" "bootargs=$lit"
lacks "fw_setenv" "a 64 MB board: left alone"
exited 0 "... and the script runs to its end"
loaded "... and loads HDAL"

run 128 32 "earlyprintk mem=32M" "osmem=32M" "bootargs=earlyprintk mem=40M panic=20"
lacks "fw_setenv" "saved bootargs already disagreeing (mem=40M): not touched"
exited 0 "... and the script runs to its end"

run 128 32 "$lit" "osmem=32M" "bootargs=earlyprintk mem=32M panic=20 mem=40M"
lacks "fw_setenv" "two mem= in saved bootargs: not touched, not half-moved"

rm -rf "$work/dt"
echo "=== the mem= / osmem guard ==="

run 128 32 "earlyprintk mem=54M panic=20" "osmem=32M" "bootargs=earlyprintk mem=54M panic=20"
has "fw_setenv bootargs earlyprintk mem=32M panic=20" "mem=54M past a 32M layout: bootargs brought back to the layout"
lacks "insmod hdal/kwrap.ko" "... and HDAL is not loaded over the kernel's memory"
lacks "fw_setenv --script" "... and the 48M move waits for a consistent boot"
exited 1 "... and the script stops there"

run 128 32 "earlyprintk panic=20 mem=54M" "osmem=32M" "bootargs=earlyprintk panic=20 mem=54M"
has "fw_setenv bootargs earlyprintk panic=20 mem=32M" "... mem= as the last argument too"

if [ "$fail" -ne 0 ]; then
	echo "$fail check(s) failed"
	exit 1
fi
echo "all load_novatek checks passed"
