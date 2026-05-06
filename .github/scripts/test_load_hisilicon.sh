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

echo
if [ "$fail" -eq 0 ]; then
    echo "All load_hisilicon regression checks passed."
    exit 0
else
    echo "$fail check(s) failed. See https://github.com/OpenIPC/firmware/issues/2059"
    exit 1
fi
