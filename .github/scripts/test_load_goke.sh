#!/bin/sh
# load_goke (gk7205v200 family): the open_osal arguments it settles on.
#
# A kernel with goke_mem measures DRAM and lays it out itself, and publishes
# the result under /sys/module/goke_mem/parameters; load_goke then loads osal
# with exactly that and never touches the U-Boot environment. Without it (an
# older kernel) the zone still comes from bootargs and totalmem/osmem.
#
# Runs the real script with no arguments -- it prints "osal: <args>" and stops
# at its usage text -- against stub fw_printenv/fw_setenv/ipcinfo/modprobe, a
# fake parameters directory and a fake command line. Under busybox sh and awk,
# as on the camera: the zone arithmetic feeds awk 0x40000000, which busybox
# awk reads as hex and gawk/mawk as 0.

set -eu

if ! command -v busybox >/dev/null 2>&1; then
	echo "FAIL busybox is needed: it is the shell and awk the camera runs"
	exit 1
fi

script=general/package/goke-osdrv-gk7205v200/files/script/load_goke
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir -p "$work/bin"

cat >"$work/bin/fw_printenv" <<'EOF'
#!/bin/sh
[ "$1" = -n ] && shift
sed -n "s/^$1=//p" "$ENV_FILE" | grep . || exit 1
EOF
cat >"$work/bin/fw_setenv" <<'EOF'
#!/bin/sh
echo "fw_setenv $*" >>"$WRITES"
EOF
cat >"$work/bin/modprobe" <<'EOF'
#!/bin/sh
echo "modprobe $*" >>"$WRITES"
EOF
cat >"$work/bin/ipcinfo" <<'EOF'
#!/bin/sh
echo gk7205v300
EOF
cat >"$work/bin/awk" <<'EOF'
#!/bin/sh
exec busybox awk "$@"
EOF
chmod +x "$work/bin/"*

# run CMDLINE ENV [DRAM OSMEM ALLOCATOR MMZ]: the osal line, writes in $work/writes
run() {
	printf '%s\n' "$1" >"$work/cmdline"
	printf '%s\n' "$2" | tr ' ' '\n' >"$work/env"
	rm -rf "$work/params" "$work/writes"
	: >"$work/writes"
	if [ $# -gt 2 ]; then
		mkdir -p "$work/params"
		echo "$3" >"$work/params/dram"
		echo "$4" >"$work/params/osmem"
		echo "$5" >"$work/params/allocator"
		echo "$6" >"$work/params/mmz"
	fi
	PATH="$work/bin:$PATH" ENV_FILE="$work/env" WRITES="$work/writes" \
		LOAD_GOKE_CMDLINE="$work/cmdline" LOAD_GOKE_MEM="$work/params" \
		busybox sh "$script" 2>&1 | sed -n 's/^osal: //p'
}

expect() {
	if [ "$1" = "$2" ]; then ok "$3"; else bad "$3 -- want '$1', got '$2'"; fi
}

no_writes() {
	if [ -s "$work/writes" ]; then bad "$1 -- wrote: $(cat "$work/writes")"; else ok "$1"; fi
}

echo "=== a kernel that lays memory out itself ==="
got=$(run "mem=39M console=ttyAMA0,115200" "osmem=32M" \
	64 32 gk anonymous,0,0x42000000,32M)
expect "anony=1 mmz_allocator=gk mmz=anonymous,0,0x42000000,32M" "$got" \
	"64M under a vendor U-Boot (mem=39M): the kernel's gk zone, not mem="
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
	"a gk7205v300 under a vendor U-Boot without totalmem: all 128M, not 64"

echo
echo "=== an older kernel: bootargs and the environment ==="
got=$(run "mem=32M console=ttyAMA0,115200 mmz_allocator=gk" "osmem=32M totalmem=128M")
expect "anony=1 mmz_allocator=gk mmz=anonymous,0,0x42000000,96M" "$got" \
	"gk: the zone from mem= to totalmem"

got=$(run "mem=128M mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "osmem=32M totalmem=128M")
expect "anony=1 mmz_allocator=cma mmz=anonymous,0,0x42000000,96M" "$got" \
	"cma: the zone on the command line"

got=$(run "mem=39M mmz_allocator=gk" "osmem=32M")
expect "anony=1 mmz_allocator=gk mmz=anonymous,0,0x42700000,25M" "$got" \
	"a vendor U-Boot's mem=39M: the zone starts above it"

echo
echo "=== the boot path never writes the environment on a goke_mem kernel ==="
if grep -q '\[ -n "\$kernel_alloc" \] || check_allocator' "$script"; then
	ok "insert_osal skips check_allocator when the kernel laid memory out"
else
	bad "insert_osal must skip check_allocator (and its fw_setenv) on a goke_mem kernel"
fi

echo
if [ "$fail" -eq 0 ]; then
	echo "All load_goke checks passed."
else
	echo "$fail check(s) failed."
	exit 1
fi
