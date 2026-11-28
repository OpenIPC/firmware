#!/bin/sh
# Regression test for load_hisilicon's os_mem_size derivation.
#
# Catches #2059: kernel cmdline mem=NM was unconditionally accepted as the
# OS/MMZ split, which broke V4+CMA boards where bootargs pass mem=<totalmem>
# (with the MMZ chunk CMA-reserved within it). Result: os_mem_size=mem_total,
# the existing "[ os_mem >= total ]" guard tripped, script exited before any
# insmod, every camera came up with empty lsmod.
#
# Two-part check:
#   Part 1 — logic test: synthetic harness reproducing the post-fix code.
#            Exercises the parsing rules directly; independent of script files.
#   Part 2 — drift test: every general/package/hisilicon-osdrv-*/.../load_hisilicon
#            must contain the validation block. If someone reverts the fix in any
#            family the drift test fails immediately.
#
# Lightweight: pure shell, no QEMU, runs in a few seconds.

set -eu

fail=0
ok()   { echo "ok   $*"; }
bad()  { echo "FAIL $*"; fail=$((fail + 1)); }
T()    { local exp="$1" act="$2" desc="$3"
         if [ "$exp" = "$act" ]; then ok "$desc"; else bad "$desc -- want '$exp', got '$act'"; fi; }

# ----- Part 1: parsing logic -----
# Mirrors the post-fix block in every load_hisilicon. Any divergence here
# (vs the scripts) is caught by Part 2.
parse_os_mem() {
    cmdline="$1"; mem_total="$2"; osmem_env="$3"
    os_mem_size=$(printf '%s' "$cmdline" | awk 'BEGIN{RS=" "} /^mem=[0-9]+M/{gsub(/^mem=|M.*$/,""); print; exit}')
    if [ -n "$os_mem_size" ] && [ "$os_mem_size" -ge "$mem_total" ]; then
        os_mem_size=""
    fi
    if [ -z "$os_mem_size" ]; then
        os_mem_size="$osmem_env"
    fi
    : "${os_mem_size:=32}"
    printf '%s\n' "$os_mem_size"
}

echo "=== Part 1: os_mem_size derivation logic ==="
# The bug case: V4+CMA cmdline passes mem=<totalmem>. Pre-fix this set
# os_mem_size=128, mem_total=128, then the load_hisilicon guard "[ os_mem
# >= total ]" exited the whole script. Post-fix the validation block
# discards the cmdline value and falls through to the osmem env (32).
T 32 "$(parse_os_mem 'mem=128M mmz_allocator=cma mmz=anonymous,0,0x42000000,96M' 128 32)" \
   "V4+CMA mem=128M, totalmem=128M → fall through to osmem env (#2059)"

# Legacy split: cmdline mem= is strictly less than totalmem, signaling
# a real OS/MMZ split. Use the cmdline value as authoritative.
T 96 "$(parse_os_mem 'mem=96M mmz_allocator=hisi mmz=anonymous,0,0x46000000,32M' 128 32)" \
   "legacy mem=96M, totalmem=128M → use cmdline 96 (PR #2039 intent)"

# No mem= at all — fall back to env.
T 32 "$(parse_os_mem 'console=ttyAMA0,115200 root=/dev/mtdblock3' 64 32)" \
   "no cmdline mem= → fall back to osmem env"

# Neither cmdline nor env — script default of 32.
T 32 "$(parse_os_mem '' 64 '')" \
   "no cmdline mem=, no env → default 32"

# Misconfigured cmdline (mem= over total). Still fall through to env to
# avoid the >= guard later in the script killing the boot.
T 64 "$(parse_os_mem 'mem=256M' 128 64)" \
   "mem=256M > totalmem=128 → fall through (avoid guard)"

# Edge: mem= equals totalmem-1 (legitimate split with 1M for MMZ — silly
# but valid). Should still use cmdline.
T 127 "$(parse_os_mem 'mem=127M' 128 32)" \
   "mem=127M, totalmem=128M → use cmdline 127"

# ----- Part 2: every hisilicon-osdrv-* script contains the validation -----
echo
echo "=== Part 2: validation block present in every load_hisilicon ==="
needle='if \[ -n "\$os_mem_size" \] && \[ "\$os_mem_size" -ge "\$mem_total" \]; then'
scripts=$(find general/package -name 'load_hisilicon' -path '*hisilicon-osdrv-*' | sort)

if [ -z "$scripts" ]; then
    bad "no hisilicon-osdrv-*/files/script/load_hisilicon found — repo layout changed?"
else
    count=$(printf '%s\n' "$scripts" | wc -l)
    echo "scanning $count load_hisilicon copies"
    for s in $scripts; do
        family=$(printf '%s\n' "$s" | sed 's|.*hisilicon-osdrv-||; s|/.*||')
        if grep -qE "$needle" "$s"; then
            ok "$family: validation block present"
        else
            bad "$family: validation block MISSING — fix from #2060 was reverted or not applied"
        fi
    done
fi

# ----- Part 3: hi3516cv100 detection retries the other sensor clocks -----
# #1352: a JXH22 does not answer at the 37.125 MHz the detection probe runs
# at, ipctool then names a TechPoint "TPffff" from the all-ones reads, and
# that name was saved for good. detect_sensor() probes again at 24 and 27 MHz
# when the first answer is not a name insert_sns can load. The functions are
# taken from the script itself, with ipcinfo and devmem stubbed: the stub
# sensor answers only at the clock it is given.
echo
echo "=== Part 3: hi3516cv100 detection clock retry (#1352) ==="
cv100=general/package/hisilicon-osdrv-hi3516cv100/files/script/load_hisilicon
fn() { sed -n "/^$1() {/,/^}/p" "$cv100"; }
# A case label line, once comments are dropped and "\" continuations joined.
labels() { fn "$1" | sed 's/#.*//' | sed -e ':a' -e '/\\$/N; s/\\\n//; ta' |
           grep -E '^[[:space:]]*[a-z0-9_|[:space:]]+\)[[:space:]]*$' |
           tr -d ' \t)' | tr '|' '\n' | sort -u; }
if [ "$(labels insert_sns)" = "$(labels sensor_known)" ]; then
    ok "cv100: sensor_known lists exactly insert_sns's case arms"
else
    bad "cv100: sensor_known and insert_sns disagree: $(labels insert_sns | tr '\n' ' ') vs $(labels sensor_known | tr '\n' ' ')"
fi

eval "$(fn sensor_known)"
eval "$(fn detect_sensor)"
devmem() { clock=$3; writes=$((writes + 1)); }
sleep()  { :; }
logger() { :; }
# Prints what detect_sensor returned and how many clock writes it made, so a
# first-probe hit that still touched the clock fails the test.
probe() {   # $1: what answers at 37.125 MHz, $2: the clock the sensor needs, $3: its name
    at37="$1"; want="$2"; name="$3"; clock=0x6; writes=0
    ipcinfo() { if [ "$clock" = "$want" ]; then echo "$name"; else echo "$at37"; fi; }
    # Not $(detect_sensor): a subshell would keep the write count to itself.
    detect_sensor > "$probe_out"
    echo "$(cat "$probe_out") writes=$writes"
}
probe_out=$(mktemp)
trap 'rm -f "$probe_out"' EXIT
T "jxh22 writes=1"  "$(probe tpffff 0x1 jxh22)"  "JXH22 silent at 37.125 MHz -> found at 24 MHz, one clock write (#1352)"
T "sc1135 writes=2" "$(probe ''     0x5 sc1135)" "nothing at 37.125 MHz -> found at 27 MHz, two clock writes"
T "imx122 writes=0" "$(probe imx122 0x6 imx122)" "IMX122 answers the first probe -> kept, no clock write"
T " writes=2"       "$(probe tpffff 0x9 jxh22)"  "no clock helps -> nothing, so the phantom tpffff is never saved"

# ----- Part 4: hi3519dv500 second sensor from the environment -----
# A sensor on SENSOR1 can be named by `fw_setenv sensor1 <name>` when it needs
# another master clock than SENSOR0's; it sets the second port's sensor type
# for open_sys_config and muxes I2C5. Without it, the port follows SENSOR0 and
# the pads are left to majestic. The functions are taken from
# the script itself, with fw_printenv and devmem stubbed.
echo
echo "=== Part 4: hi3519dv500 SENSOR1 from the environment ==="
dv500=general/package/hisilicon-osdrv-hi3519dv500/files/script/load_hisilicon
fn5() { sed -n "/^$1() {/,/^}/p" "$dv500"; }
eval "$(fn5 sensor1_from_env)"
eval "$(fn5 sensor1_pinmux)"
devmem() { pinmux="$pinmux $1=$3"; }
sensor1() {   # $1: the sensor1 env value, $2: -sensor1 given (1) or not (0)
    env1="$1"; SNS_SET1="$2"; SNS_TYPE1=${3:-sc450ai}; pinmux=""
    fw_printenv() { [ -n "$env1" ] && echo "$env1"; }
    sensor1_from_env
    [ "$SNS_SET1" -eq 0 ] && SNS_TYPE1=os04a10   # what SENSOR0 resolved to
    sensor1_pinmux
    echo "$SNS_TYPE1$pinmux"
}
T "os08a20 0x0EFF0024=0x1752 0x0EFF0028=0x1752" "$(sensor1 os08a20 0)" \
   "sensor1=os08a20 in the environment -> SNS_TYPE1 os08a20, I2C5 muxed"
T "os04a10" "$(sensor1 '' 0)" \
   "no sensor1 -> SNS_TYPE1 follows SENSOR0, SPI1 pins left alone"
T "imx515 0x0EFF0024=0x1752 0x0EFF0028=0x1752" "$(sensor1 os08a20 1 imx515)" \
   "-sensor1 given -> it wins over the environment, I2C5 muxed"

# ----- Part 5: hi3519dv500 re-detects a swapped sensor -----
# The sensors on this board are detachable, so the saved `sensor` variable is
# checked against the bus on every boot: what answers wins and is saved when it
# differs, the saved name stands when nothing answers, and -sensor0 skips the
# probe. resolve_sensor is taken from the script itself, with the environment,
# the probe and the module loading stubbed; probes and writes are counted.
echo
echo "=== Part 5: hi3519dv500 sensor re-detected every boot ==="
eval "$(fn5 resolve_sensor)"
insert_detect() { :; }
remove_detect() { :; }
probe_log=$(mktemp)
trap 'rm -f "$probe_out" "$probe_log"' EXIT
# Not "saved"/"detected": resolve_sensor declares those local, and the stubs
# run inside it. The probe runs in $(...), so it counts into a file.
resolve() {   # $1: saved `sensor`, $2: what answers, $3: SENSOR on entry, $4: -sensor0 given
    env_sensor="$1"; on_bus="$2"; SENSOR="$3"; SNS_SET0=${4:-0}; writes=""
    : > "$probe_log"
    fw_printenv() { [ -n "$env_sensor" ] && echo "$env_sensor"; }
    fw_setenv() { writes="$writes $1=${2:-}"; }
    ipcinfo() { echo x >> "$probe_log"; echo "$on_bus"; }
    resolve_sensor > /dev/null
    echo "$SENSOR probes=$(wc -l < "$probe_log") writes=${writes# }"
}
T "os04a10 probes=1 writes=" "$(resolve os04a10 os04a10 os04a10)" \
   "saved sensor still fitted -> kept, nothing written"
T "os08a10 probes=1 writes=sensor=os08a10" "$(resolve os04a10 os08a10 os04a10)" \
   "sensor swapped -> the one that answers wins and is saved"
T "os04a10 probes=1 writes=" "$(resolve os04a10 '' os04a10)" \
   "nothing answers -> the saved name stands, nothing written"
T "unknown probes=1 writes=" "$(resolve '' '' '')" \
   "nothing saved, nothing answers -> unknown, never saved"
T "os04a10 probes=1 writes=sensor= sensor=os04a10" "$(resolve unknown os04a10 unknown)" \
   "a saved 'unknown' is dropped and the probe's answer saved (#2428)"
T "imx515 probes=0 writes=" "$(resolve os04a10 os08a10 imx515 1)" \
   "-sensor0 given -> taken as given, no probe"

# ----- Part 6: hi3519dv500 video output follows the HDMI bridge -----
# BT.1120 is muxed and the VO modules loaded only when the LT8618SXB answers on
# I2C7 (chip ID 0x17 0x02), or when `vo_intf` in the environment names an
# interface; `none` turns video output off. A bus where nothing answers at
# 0x39 gets no writes, and another chip there gets back what was in the two
# registers the probe wrote. A bridge driver that does not load is reported.
# The functions are taken from the script itself, with the I2C tools, devmem,
# the environment, logger and modprobe stubbed.
echo
echo "=== Part 6: hi3519dv500 video output follows the HDMI bridge ==="
eval "$(fn5 hdmi_bridge_present)"
eval "$(fn5 resolve_vo)"
eval "$(fn5 vo_sys_config_arg)"
eval "$(fn5 insert_vo)"
devmem() { :; }
# 0x39 is modelled as a register file: the ID registers and 0xff/0xee, which
# start at 0x5a/0x3c so a write that is not undone shows.
vo() {   # $1: vo_intf in the environment, $2: chip ID at 0x39 ("" = no ACK), $3: bridge driver fails
    env_vo="$1"; at39="$2"; drv_fails="${3:-0}"; VO_INTF=""; i2c_writes=0; loaded=""; logged=""
    reg_ff=0x5a; reg_ee=0x3c
    fw_printenv() { [ -n "$env_vo" ] && echo "$env_vo"; }
    i2cget() {
        [ -n "$at39" ] || return 1
        case $4 in 0x00) echo "${at39% *}" ;; 0x01) echo "${at39#* }" ;;
            0xff) echo "$reg_ff" ;; 0xee) echo "$reg_ee" ;; esac
    }
    i2cset() {
        i2c_writes=$((i2c_writes + 1))
        case $4 in 0xff) reg_ff=$5 ;; 0xee) reg_ee=$5 ;; esac
    }
    # The bridge driver is loaded inside $(...), to keep what modprobe says,
    # so loads are counted into a file.
    : > "$probe_log"
    modprobe() {
        printf ' %s' "$1" >> "$probe_log"
        [ "$1" = open_lt8618sx ] && [ "$drv_fails" = 1 ] && { echo "no such device" >&2; return 1; }
        return 0
    }
    logger() { case "$*" in *daemon.err*) logged=err ;; esac; }
    resolve_vo
    insert_vo
    loaded=$(cat "$probe_log")
    echo "[$(vo_sys_config_arg)] writes=$i2c_writes regs=$reg_ff/$reg_ee loaded=${loaded# }${logged:+ logged=$logged}"
}
T "[vo_intf=bt1120] writes=2 regs=0x80/0x01 loaded=open_vo_dev open_vo open_lt8618sx" "$(vo '' '0x17 0x02')" \
   "bridge answers -> BT.1120, VO and the bridge driver loaded"
T "[] writes=0 regs=0x5a/0x3c loaded=" "$(vo '' '')" \
   "nothing at 0x39 -> no VO, and nothing written to the bus"
T "[] writes=4 regs=0x5a/0x3c loaded=" "$(vo '' '0x55 0x01')" \
   "another chip at 0x39 -> not the bridge, no VO, its registers put back"
T "[vo_intf=bt1120] writes=2 regs=0x80/0x01 loaded=open_vo_dev open_vo open_lt8618sx logged=err" "$(vo '' '0x17 0x02' 1)" \
   "bridge driver fails to load -> reported, the boot goes on"
T "[] writes=0 regs=0x5a/0x3c loaded=" "$(vo none '0x17 0x02')" \
   "vo_intf=none -> no VO even with the bridge, no probe"
T "[vo_intf=mipi_tx] writes=0 regs=0x5a/0x3c loaded=open_vo_dev open_vo open_mipi_tx" "$(vo mipi_tx '')" \
   "vo_intf=mipi_tx -> taken as given, MIPI TX loaded, no probe"

echo
if [ "$fail" -eq 0 ]; then
    echo "All load_hisilicon regression checks passed."
    exit 0
else
    echo "$fail check(s) failed. See https://github.com/OpenIPC/firmware/issues/2059"
    exit 1
fi
