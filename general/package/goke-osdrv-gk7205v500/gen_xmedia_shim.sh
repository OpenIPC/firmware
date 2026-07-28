#!/bin/sh
# gen_xmedia_shim.sh - generate the OpenIPC-facade -> XMEDIA forwarding shims.
#
# THIS SCRIPT PRODUCED files/lib/libhi_{mpi,ae,awb,isp,ive,md}.so, which are checked into
# this package as binaries. It is kept here so those six blobs are reproducible rather than
# unexplained: re-run it against the vendor libraries to regenerate them.
#
# WHAT THIS IS
# ------------
# OpenIPC already ships exactly this shim, for the GK stack:
#
#     libgk_api.so   309 KB   533 x GK_API_*   defined     <- implementation
#     libhi_mpi.so    70 KB   533 x HI_MPI_*   defined     <- facade, pure tail branches
#                             533 x GK_API_*   undefined   <- forwards to libgk_api.so
#
# Six such pairs exist (mpi/api, ae, awb, isp, ive, md) and Majestic NEEDs all twelve.
# This builds the same six facades against the XMEDIA (2023) implementation instead:
#
#     libhi_mpi.so -> libxmedia_api.so      libhi_ive.so -> libxmedia_ive.so
#     libhi_ae.so  -> libxmedia_ae.so       libhi_md.so  -> libxmedia_md.so
#     libhi_awb.so -> libxmedia_awb.so      libhi_isp.so -> libxmedia_isp.so
#
# WHY IT WORKS
# ------------
# Each stub is a single tail branch:
#
#     HI_MPI_VENC_CreateChn:  b  XMEDIA_API_VENC_CreateChn
#
# `b` does not touch lr, sp, or any argument register. The callee returns straight to Majestic.
# ABI-transparent WITHOUT knowing any signature - no prototypes, no structs, no guesses.
# Same reason patch_sc2336_syms.py worked: HI_MPI / GK_API / XMEDIA_API are one SDK, three names.
#
# BUGS IN v1 THAT THIS FIXES
# --------------------------
# 1. NAMING RULE WAS ASSUMED, NOT READ. v1 grepped only HI_MPI_ -> XMEDIA_API_. Measured:
#
#        libhi_md.so      HI_IVS_MD_CreateChn @0x800    (8 stubs - already a facade)
#        libxmedia_md.so  IVS_MD_CreateChn    @0x1bd4   (the implementation)
#
#    That rule is HI_ -> nothing, not HI_MPI_ -> XMEDIA_API_. libhi_md.so silently produced
#    0 stubs. This version does not hardcode any rule: it reads every symbol the ORIGINAL
#    facade defines and tries an ordered list of transforms against what the XMEDIA library
#    actually exports. Anything unmatched is printed, never skipped quietly.
#
# 2. `set -e` KILLED THE VERIFY LOOP. grep returns 1 on no-match; the loop ran inside a
#    pipeline (= subshell, inherits -e), hit the empty libhi_md.so, and died BEFORE reaching
#    libhi_mpi.so. Reported "provide: 219" (= ae+awb+isp+ive exactly) and a 110-line unresolved
#    list that was pure artifact. The shims were fine; the check lied. Fixed with `|| true`.
#
# 3. MISSING .note.GNU-stack -> the linker assumed an executable stack for the whole library.
#    Hand-written .S must declare it. Fixed by emitting the section.
#
# 4. WRONG QUESTION. v1 verified "are all HI_MPI_* covered". The real question is "does every
#    symbol Majestic imports FROM THESE SIX LIBRARIES still resolve" - which is what catches
#    HI_IVS_MD_* and anything else no one thought to grep for. This version asks that.
#
# USAGE
#   cd /work/firmware
#   sh gen_xmedia_shim2.sh
#   # -> /tmp/xmshim2/lib/libhi_{mpi,ae,awb,isp,ive,md}.so
#
# Override the osdrv library dir if it moved:  LDIR=/path/to/lib sh gen_xmedia_shim2.sh

set -u          # NOTE: deliberately NOT -e. grep returning 1 is normal here, not fatal.

FW=${FW:-$(pwd)}
NM=${NM:-$FW/output/host/bin/arm-openipc-linux-musleabi-nm}
CC=${CC:-$FW/output/host/bin/arm-openipc-linux-musleabi-gcc}
T=${T:-$FW/output/target/usr/lib}
LDIR=${LDIR:-$FW/general/package/goke-osdrv-gk7205v500/files/lib}
MAJ=${MAJ:-$FW/output/target/usr/bin/majestic}
OUT=${OUT:-/tmp/xmshim2}

[ -x "$NM" ]   || { echo "no nm at $NM";     exit 1; }
[ -x "$CC" ]   || { echo "no gcc at $CC";    exit 1; }
[ -d "$LDIR" ] || { echo "no LDIR $LDIR";    exit 1; }
[ -f "$MAJ" ]  || { echo "no majestic at $MAJ"; exit 1; }

rm -rf "$OUT"
mkdir -p "$OUT/asm" "$OUT/lib" "$OUT/sym"

# Exported code symbols of an .so, one per line. T = global text, W = weak text.
# Linker-generated and internal names are not API and must not be forwarded.
defined_text() {
    "$NM" -D --defined-only "$1" 2>/dev/null \
        | awk '$(NF-1)=="T" || $(NF-1)=="W" { print $NF }' \
        | grep -vE '^(_init|_fini|__.*|_edata|_end|_bss_start)$' \
        | sort -u
}

# Exported DATA symbols. A `b` branch cannot forward data - it would branch to the bytes.
# If any exist we must know, because they are a real hole in the shim approach.
defined_data() {
    "$NM" -D --defined-only "$1" 2>/dev/null \
        | awk '$(NF-1)=="D" || $(NF-1)=="B" || $(NF-1)=="R" { print $NF }' \
        | grep -vE '^(__.*|_edata|_end)$' \
        | sort -u
}

# hi-facade : xmedia-implementation
PAIRS="mpi:api ae:ae awb:awb isp:isp ive:ive md:md"

total=0
totmiss=0

for p in $PAIRS; do
    hi=${p%%:*}
    xm=${p##*:}
    src="$T/libhi_${hi}.so"
    tgt="$LDIR/libxmedia_${xm}.so"

    if [ ! -f "$src" ]; then echo "-- skip libhi_${hi}.so (not in target)";  continue; fi
    if [ ! -f "$tgt" ]; then echo "-- skip libxmedia_${xm}.so (not in osdrv)"; continue; fi

    defined_text "$src" > "$OUT/sym/hi_$hi.txt"
    defined_text "$tgt" > "$OUT/sym/xm_$xm.txt"

    # data exports are a hole in the tail-branch approach - report loudly, do not hide
    dat=$(defined_data "$src")
    if [ -n "$dat" ]; then
        echo "   ** libhi_${hi}.so exports DATA symbols - a branch cannot forward these:"
        echo "$dat" | sed 's/^/      /'
    fi

    n=0
    miss=0

    {
        echo '  .text'
        while read -r s; do
            [ -n "$s" ] || continue

            # Ordered transforms. First hit wins. No rule is assumed to be THE rule -
            # both known naming conventions are tried, plus identity.
            #   HI_MPI_VENC_CreateChn -> XMEDIA_API_VENC_CreateChn   (mpi/ae/awb/isp/ive)
            #   HI_IVS_MD_CreateChn   -> IVS_MD_CreateChn            (md)
            impl=""
            for cand in \
                "$(printf '%s' "$s" | sed 's/^HI_MPI_/XMEDIA_API_/')" \
                "$(printf '%s' "$s" | sed 's/^HI_//')" \
                "$s"
            do
                if grep -qxF "$cand" "$OUT/sym/xm_$xm.txt"; then impl=$cand; break; fi
            done

            if [ -n "$impl" ]; then
                printf '  .global %s\n  .type %s,%%function\n%s:\n  b %s\n  .size %s,.-%s\n' \
                       "$s" "$s" "$s" "$impl" "$s" "$s"
                n=$((n + 1))
                echo "$s -> $impl" >> "$OUT/sym/map_$hi.txt"
            else
                miss=$((miss + 1))
                echo "$s" >> "$OUT/sym/miss_$hi.txt"
            fi
        done < "$OUT/sym/hi_$hi.txt"

        # Hand-written asm must declare a non-executable stack or the linker assumes the
        # worst and marks the whole library RWX.
        echo '  .section .note.GNU-stack,"",%progbits'
    } > "$OUT/asm/shim_$hi.S"

    # -shared: each `b <impl>` becomes a PLT tail-branch into libxmedia_$xm.so.
    # The PLT veneer clobbers only ip (r12), which is call-clobbered. lr is untouched, so the
    # callee returns directly to Majestic. Signatures never enter into it.
    if "$CC" -shared -fPIC -o "$OUT/lib/libhi_${hi}.so" "$OUT/asm/shim_$hi.S" \
             -Wl,-soname,"libhi_${hi}.so" -Wl,-z,noexecstack \
             -L "$LDIR" -l:"libxmedia_${xm}.so" 2>"$OUT/sym/ld_$hi.err"
    then
        st=ok
    else
        st=FAILED
        echo "   !! link failed for libhi_${hi}.so:"
        sed 's/^/      /' "$OUT/sym/ld_$hi.err"
    fi

    printf 'libhi_%-4s -> libxmedia_%-4s : %4d forwarded, %2d unmatched   [%s]\n' \
           "$hi" "$xm" "$n" "$miss" "$st"
    [ "$miss" -gt 0 ] && sed 's/^/      unmatched: /' "$OUT/sym/miss_$hi.txt"

    total=$((total + n))
    totmiss=$((totmiss + miss))
done

echo ""
echo "total forwarded: $total   total unmatched: $totmiss"
ls -la "$OUT/lib/"

# ---------------------------------------------------------------------------
# VERIFY - the question that matters
# ---------------------------------------------------------------------------
# Not "are all HI_MPI_* covered" (v1's question, which missed HI_IVS_MD_* entirely) but:
# every symbol Majestic imports that USED to come from one of these six libraries must still
# be defined by our replacements. Prefix-agnostic by construction.
echo ""
echo "=== verify ==="

"$NM" -D --undefined-only "$MAJ" 2>/dev/null | awk '{print $NF}' | sort -u > "$OUT/need_all.txt"

: > "$OUT/orig_all.txt"
for p in $PAIRS; do
    hi=${p%%:*}
    [ -f "$T/libhi_${hi}.so" ] && defined_text "$T/libhi_${hi}.so" >> "$OUT/orig_all.txt"
done
sort -u -o "$OUT/orig_all.txt" "$OUT/orig_all.txt"

: > "$OUT/provide_all.txt"
for f in "$OUT"/lib/*.so; do
    [ -f "$f" ] && defined_text "$f" >> "$OUT/provide_all.txt"
done
sort -u -o "$OUT/provide_all.txt" "$OUT/provide_all.txt"

# what majestic actually takes from these six libraries
comm -12 "$OUT/need_all.txt" "$OUT/orig_all.txt" > "$OUT/need_from_six.txt"

echo "majestic imports total          : $(wc -l < "$OUT/need_all.txt")"
echo "  ...of which from these six    : $(wc -l < "$OUT/need_from_six.txt")"
echo "original six define             : $(wc -l < "$OUT/orig_all.txt")"
echo "our shims define                : $(wc -l < "$OUT/provide_all.txt")"
echo ""
echo "--- UNRESOLVED (must be empty) ---"
comm -23 "$OUT/need_from_six.txt" "$OUT/provide_all.txt" | tee "$OUT/unresolved.txt"
u=$(wc -l < "$OUT/unresolved.txt")
echo "--- $u unresolved ---"

# Symbols the original facade had but we could not map. Harmless unless majestic imports
# them - which the UNRESOLVED list above already decides. Listed for the record only.
echo ""
echo "--- dropped vs original (informational; only matters if listed above too) ---"
comm -23 "$OUT/orig_all.txt" "$OUT/provide_all.txt" | sed 's/^/  /'

echo ""
if [ "$u" -eq 0 ] && [ "$totmiss" -eq 0 ]; then
    echo "RESULT: complete. Install $OUT/lib/*.so over the target's libhi_*.so."
elif [ "$u" -eq 0 ]; then
    echo "RESULT: sufficient. $totmiss symbol(s) unmatched but majestic imports none of them."
else
    echo "RESULT: INCOMPLETE - $u symbol(s) majestic needs are not provided. Do not flash."
fi
