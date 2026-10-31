#!/bin/sh
# load_hisilicon (hi3516ev200, hi3516cv6xx, hi3519dv500): the media memory
# zone it loads, and that it loads it without writing the U-Boot environment;
# and that set_allocator, the user's override, writes only bootargs that boot.
#
# A kernel with hisi_mem measures DRAM and lays it out itself, and publishes
# the result under /sys/module/hisi_mem/parameters; load_hisilicon then loads
# the zone module (open_osal on hi3516ev200, open_mmz on the V5 parts) with
# exactly that. Without it (an older kernel) the zone still comes from bootargs
# and totalmem/osmem.
#
# Runs the real script the way S70vendor does at boot (load_hisilicon -i, here
# with the sensor given so nothing is probed), under busybox sh and awk as on
# the camera: the zone arithmetic feeds awk 0x40000000, which busybox awk reads
# as hex and gawk/mawk as 0. The script is sourced from a wrapper that defines
# modprobe, fw_setenv, fw_printenv and ipcinfo as shell functions -- busybox
# sh runs its own applets ahead of anything on PATH, but not ahead of
# functions -- so every module load and environment write is recorded.

set -eu

if ! command -v busybox >/dev/null 2>&1; then
	echo "FAIL busybox is needed: it is the shell and awk the camera runs"
	exit 1
fi

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
ipcinfo() { echo "$CHIP"; }
logger() { :; }
set -- -i -sensor "$SENSOR"
. "$SCRIPT"
EOF
# set_allocator is a script of its own, called from the older kernel's path
cat >"$work/bin/set_allocator" <<'EOF'
#!/bin/sh
echo "set_allocator $*" >>"$CALLS"
EOF
chmod +x "$work/bin/set_allocator"
echo "40000000-4fffffff : System RAM" >"$work/iomem"

# run CMDLINE ENV [DRAM OSMEM ALLOCATOR MMZ]: the zone module's arguments;
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
		CHIP="$chip" SENSOR="$sensor" ENV_FILE="$work/env" \
		LOAD_HISILICON_CMDLINE="$work/cmdline" LOAD_HISILICON_MEM="$work/params" \
		LOAD_HISILICON_IOMEM="$work/iomem" \
		busybox sh "$work/wrapper" >"$work/out" 2>&1) || true
	sed -n "s/^modprobe $module //p" "$work/calls" | sed 's/ mem_process_isolation=[0-9]*$//'
}

cat >"$work/sa_wrapper" <<'EOF'
fw_setenv() { echo "fw_setenv $*" >>"$CALLS"; }
fw_printenv() { [ "$1" = -n ] && shift; sed -n "s/^$1=//p" "$ENV_FILE" | grep . || return 1; }
. "$SCRIPT"
EOF

# sa ENV DT [DRAM OSMEM ALLOCATOR MMZ] -- ARGS: set_allocator ARGS; prints the
# bootargs it wrote, or "refused" when it wrote none. ENV is "key=value" lines
# joined by '|'; DT the memory node's reg as printf escapes ('' for none).
sa() {
	printf '%s\n' "$1" | tr '|' '\n' >"$work/env"
	rm -f "$work/dtmem"
	[ -z "$2" ] || printf "$2" >"$work/dtmem"
	shift 2
	rm -rf "$work/params"
	: >"$work/calls"
	if [ "$1" != -- ]; then
		mkdir -p "$work/params"
		echo "$1" >"$work/params/dram"
		echo "$2" >"$work/params/osmem"
		echo "$3" >"$work/params/allocator"
		echo "$4" >"$work/params/mmz"
		shift 4
	fi
	shift
	(cd "$work" && SCRIPT="$sa_script" CALLS="$work/calls" ENV_FILE="$work/env" \
		SET_ALLOCATOR_MEM="$work/params" SET_ALLOCATOR_DT_MEMORY="$work/dtmem" \
		busybox sh "$work/sa_wrapper" "$@" >"$work/out" 2>&1) || true
	if grep -q '^fw_setenv bootargs ' "$work/calls"; then
		sed -n 's/^fw_setenv bootargs //p' "$work/calls"
	else
		echo refused
	fi
}

# check_set_allocator PACKAGE CARVE: the user's override, on a kernel with
# hisi_mem (512M, cma, OS 128M) and on an older one
check_set_allocator() {
	sa_script=$(pwd)/general/package/$1/files/script/set_allocator
	carve=$2
	env="bootargs=console=ttyAMA0,115200 mem=32M|osmem=32M"
	params="512 128 cma anonymous,0,0x48000000,384M"

	echo
	echo "=== $1: set_allocator ==="
	expect "mem=48M console=ttyAMA0,115200 mmz_allocator=$carve osmem=48M" \
		"$(sa "$env" '' $params -- "$carve" 48)" \
		"$carve 48: whole MiB, a bare number taken as MiB"
	expect refused "$(sa "$env" '' $params -- "$carve" 48K)" \
		"$carve 48K: refused, not written as a byte count"
	expect refused "$(sa "$env" '' $params -- "$carve" 16M)" \
		"$carve 16M: refused, below the 32M the kernel takes"
	expect refused "$(sa "$env" '' $params -- "$carve" 508M)" \
		"$carve 508M of 512M: refused, nothing left for the zone"
	expect "mem=512M console=ttyAMA0,115200 mmz_allocator=cma mmz=anonymous,0,0x48000000,384M" \
		"$(sa "$env" '' $params -- cma)" \
		"cma: the zone above the OS share this boot runs with"
	# An older kernel, no totalmem: the size from the memory node. arm64's
	# two cells, as a bootloader that keeps 0x30000 for ATF fixes them up.
	expect "mem=512M console=ttyAMA0,115200 mmz_allocator=cma mmz=anonymous,0,0x42000000,480M" \
		"$(sa "$env" '\0\0\0\0\100\003\0\0\0\0\0\0\037\375\0\0' -- cma)" \
		"an older arm64 kernel: 512M from a two-cell memory node"
	expect "mem=64M console=ttyAMA0,115200 mmz_allocator=cma mmz=anonymous,0,0x42000000,32M" \
		"$(sa "$env" '\100\0\0\0\040\0\0\0' -- cma)" \
		"an older 32-bit kernel: a one-cell 512M is the placeholder, not the DRAM"
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

# check PACKAGE CHIP SENSOR MODULE CARVE LEGACY_PICKS
#   MODULE        the module that takes the zone
#   CARVE         the carve-out allocator's name
#   LEGACY_PICKS  yes when an older kernel's path still runs set_allocator
check() {
	script=$(pwd)/general/package/$1/files/script/load_hisilicon
	chip=$2 sensor=$3 module=$4 carve=$5 legacy_picks=$6

	echo
	echo "=== $1: a kernel that lays memory out itself ==="
	got=$(run "mem=39M console=ttyAMA0,115200" "osmem=32M" \
		64 32 "$carve" anonymous,0,0x42000000,32M)
	expect "anony=1 mmz_allocator=$carve mmz=anonymous,0,0x42000000,32M" "$got" \
		"64M under a vendor U-Boot (mem=39M): the kernel's zone, not mem="
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
		"a vendor U-Boot without totalmem: all 128M, not 64"

	echo "=== $1: an older kernel: bootargs and the environment ==="
	got=$(run "mem=32M console=ttyAMA0,115200 mmz_allocator=$carve" "osmem=32M totalmem=128M")
	expect "anony=1 mmz_allocator=$carve mmz=anonymous,0,0x42000000,96M" "$got" \
		"$carve: the zone from mem= to totalmem"

	got=$(run "mem=128M mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "osmem=32M totalmem=128M")
	expect "anony=1 mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "$got" \
		"cma: the zone on the command line"

	got=$(run "mem=39M mmz_allocator=$carve" "osmem=32M")
	expect "anony=1 mmz_allocator=$carve mmz=anonymous,0,0x42700000,25M" "$got" \
		"a vendor U-Boot's mem=39M: the zone starts above it"

	got=$(run "mem=32M console=ttyAMA0,115200" "osmem=32M totalmem=128M")
	expect "anony=1 mmz_allocator=$carve mmz=anonymous,0,0x42000000,96M" "$got" \
		"no mmz_allocator yet: this boot on the $carve zone"
	if [ "$legacy_picks" = yes ]; then
		if grep -q '^set_allocator cma' "$work/calls"; then
			ok "an older kernel without mmz_allocator still has set_allocator pick it"
		else
			bad "an older kernel without mmz_allocator: set_allocator not run -- $(tr '\n' ';' <"$work/calls")"
		fi
	else
		no_writes "an older kernel: the environment is not written either"
	fi
}

check hisilicon-osdrv-hi3516ev200 hi3516ev300 imx307 open_osal hisi yes
check hisilicon-osdrv-hi3516cv6xx hi3516cv610 sc4336p open_mmz ot no
check hisilicon-osdrv-hi3519dv500 hi3519dv500 sc450ai open_mmz ot no

check_set_allocator hisilicon-osdrv-hi3516ev200 hisi
check_set_allocator hisilicon-osdrv-hi3516cv6xx ot
check_set_allocator hisilicon-osdrv-hi3519dv500 ot

# hybrid, cv6xx only: a CMA zone above the OS share, the carve-out above it.
# The default layout is the one run on a 64M hi3516cv608 (OS 36M, CMA 8M,
# carve-out 20M).
sa_script=$(pwd)/general/package/hisilicon-osdrv-hi3516cv6xx/files/script/set_allocator
env="bootargs=console=ttyAMA0,115200 mem=36M|osmem=36M"
params="64 36 ot anonymous,0,0x42400000,28M"
echo
echo "=== hisilicon-osdrv-hi3516cv6xx: set_allocator hybrid ==="
expect "mem=64M console=ttyAMA0,115200 mmz_allocator=hybrid mmz=cma,0,0x42400000,8M:anonymous,0,0x42c00000,20M" \
	"$(sa "$env" '' $params -- hybrid)" \
	"hybrid: 8M of CMA above the OS share this boot runs with, the rest a carve-out"
expect "mem=64M console=ttyAMA0,115200 mmz_allocator=hybrid mmz=cma,0,0x42000000,12M:anonymous,0,0x42c00000,20M" \
	"$(sa "$env" '' $params -- hybrid 32M 12M)" \
	"hybrid 32M 12M: the OS share and the CMA share as asked"
expect "mem=64M console=ttyAMA0,115200 mmz_allocator=hybrid mmz=cma,0,0x42400000,8M:anonymous,0,0x42c00000,20M" \
	"$(sa "$env" '' $params -- hybrid 36M 6M)" \
	"hybrid 36M 6M: the CMA share rounded up to whole 4M"
expect refused "$(sa "$env" '' $params -- hybrid 36M 28M)" \
	"hybrid 36M 28M: refused, no carve-out left"
expect refused "$(sa "$env" '' $params -- hybrid 36M 0M)" \
	"hybrid 36M 0M: refused, that is ot"

echo
if [ "$fail" -eq 0 ]; then
	echo "All load_hisilicon layout checks passed."
else
	echo "$fail check(s) failed."
	exit 1
fi
