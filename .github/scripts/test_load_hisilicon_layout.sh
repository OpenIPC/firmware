#!/bin/sh
# load_hisilicon (hi3516ev200 family): the open_osal arguments it settles on.
#
# A kernel with hisi_mem measures DRAM and lays it out itself, and publishes
# the result under /sys/module/hisi_mem/parameters; load_hisilicon then loads osal
# with exactly that and never touches the U-Boot environment. Without it (an
# older kernel) the zone still comes from bootargs and totalmem/osmem.
#
# Runs the real script the way S70vendor does at boot (load_hisilicon -i, here with
# the sensor given so nothing is probed), under busybox sh and awk as on the
# camera: the zone arithmetic feeds awk 0x40000000, which busybox awk reads as
# hex and gawk/mawk as 0. The script is sourced from a wrapper that defines
# modprobe, fw_setenv, fw_printenv and ipcinfo as shell functions -- busybox
# sh runs its own applets ahead of anything on PATH, but not ahead of
# functions -- so every module load and environment write is recorded.

set -eu

if ! command -v busybox >/dev/null 2>&1; then
	echo "FAIL busybox is needed: it is the shell and awk the camera runs"
	exit 1
fi

script=$(pwd)/general/package/hisilicon-osdrv-hi3516ev200/files/script/load_hisilicon
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir -p "$work/bin"

cat >"$work/wrapper" <<'EOF'
modprobe() { echo "modprobe $*" >>"$CALLS"; }
fw_setenv() { echo "fw_setenv $*" >>"$CALLS"; }
fw_printenv() { [ "$1" = -n ] && shift; sed -n "s/^$1=//p" "$ENV_FILE" | grep . || return 1; }
ipcinfo() { echo hi3516ev300; }
logger() { :; }
set -- -i -sensor imx307
. "$SCRIPT"
EOF
# set_allocator is a script of its own, called from the older kernel's path
cat >"$work/bin/set_allocator" <<'EOF'
#!/bin/sh
echo "set_allocator $*" >>"$CALLS"
EOF
chmod +x "$work/bin/set_allocator"

# run CMDLINE ENV [DRAM OSMEM ALLOCATOR MMZ]: the open_osal arguments loaded;
# every call is left in $work/calls
run() {
	printf '%s\n' "$1" >"$work/cmdline"
	printf '%s\n' "$2" | tr ' ' '\n' >"$work/env"
	rm -rf "$work/params"
	: >"$work/calls"
	if [ $# -gt 2 ]; then
		mkdir -p "$work/params"
		echo "$3" >"$work/params/dram"
		echo "$4" >"$work/params/osmem"
		echo "$5" >"$work/params/allocator"
		echo "$6" >"$work/params/mmz"
	fi
	(cd "$work" && PATH="$work/bin:$PATH" SCRIPT="$script" CALLS="$work/calls" \
		ENV_FILE="$work/env" LOAD_HISILICON_CMDLINE="$work/cmdline" LOAD_HISILICON_MEM="$work/params" \
		busybox sh "$work/wrapper" >"$work/out" 2>&1) || true
	sed -n 's/^modprobe open_osal //p' "$work/calls"
}

expect() {
	if [ "$1" = "$2" ]; then ok "$3"; else bad "$3 -- want '$1', got '$2'"; fi
}

no_writes() {
	if grep -q -E '^(fw_setenv|set_allocator) ' "$work/calls"; then
		bad "$1 -- wrote: $(grep -E '^(fw_setenv|set_allocator) ' "$work/calls" | tr '\n' ';')"
	else
		ok "$1"
	fi
}

echo "=== a kernel that lays memory out itself ==="
got=$(run "mem=39M console=ttyAMA0,115200" "osmem=32M" \
	64 32 hisi anonymous,0,0x42000000,32M)
expect "anony=1 mmz_allocator=hisi mmz=anonymous,0,0x42000000,32M" "$got" \
	"64M under a vendor U-Boot (mem=39M): the kernel's hisi zone, not mem="
no_writes "64M: the environment is not written"

got=$(run "mem=32M console=ttyAMA0,115200" "osmem=32M totalmem=128M" \
	128 32 cma anonymous,0,0x42000000,96M)
expect "anony=1 mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "$got" \
	"128M with OpenIPC's default bootargs: cma on the first boot, no reboot"
no_writes "128M: the environment is not written"

got=$(run "mem=128M mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "osmem=32M totalmem=128M" \
	128 32 cma anonymous,0,0x42000000,96M)
expect "anony=1 mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "$got" \
	"128M with an explicit mmz= from set_allocator: the zones the kernel declared"

got=$(run "mem=39M" "" 128 32 cma anonymous,0,0x42000000,96M)
expect "anony=1 mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "$got" \
	"an hi3516ev300 under a vendor U-Boot without totalmem: all 128M, not 64"

echo
echo "=== an older kernel: bootargs and the environment ==="
got=$(run "mem=32M console=ttyAMA0,115200 mmz_allocator=hisi" "osmem=32M totalmem=128M")
expect "anony=1 mmz_allocator=hisi mmz=anonymous,0,0x42000000,96M" "$got" \
	"hisi: the zone from mem= to totalmem"

got=$(run "mem=128M mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "osmem=32M totalmem=128M")
expect "anony=1 mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "$got" \
	"cma: the zone on the command line"

got=$(run "mem=39M mmz_allocator=hisi" "osmem=32M")
expect "anony=1 mmz_allocator=hisi mmz=anonymous,0,0x42700000,25M" "$got" \
	"a vendor U-Boot's mem=39M: the zone starts above it"

got=$(run "mem=32M console=ttyAMA0,115200" "osmem=32M totalmem=128M")
expect "anony=1 mmz_allocator=hisi mmz=anonymous,0,0x42000000,96M" "$got" \
	"no mmz_allocator yet: this boot on the hisi zone"
if grep -q '^set_allocator cma' "$work/calls"; then
	ok "an older kernel without mmz_allocator still has set_allocator pick it"
else
	bad "an older kernel without mmz_allocator: set_allocator not run -- $(tr '\n' ';' <"$work/calls")"
fi

echo
if [ "$fail" -eq 0 ]; then
	echo "All load_hisilicon layout checks passed."
else
	echo "$fail check(s) failed."
	exit 1
fi
