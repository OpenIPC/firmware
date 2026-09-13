#!/bin/bash
# Regression tests for the Ethernet MAC the camera gives itself.
#
# Catches the duplicate-identity class of bug. Three separate things put one
# address on every camera of a family: HiSilicon u-boot bakes
# 00:00:23:34:45:66 into its board headers (CONFIG_ETHADDR), a full-image
# flash carries no environment at all so nothing overrides it (#2405), and the
# overlay used to fall back to the same literal in interfaces.d/eth0 and in
# S40network. It is not cosmetic: S96onvifserver derives serial_num from the
# interface MAC, so two cameras sharing an address also claim one ONVIF
# identity, and a DHCP server has one lease to give them between them.
#
# The repair that existed before this (#1147) matched ONE literal, on
# hisilicon/goke only. A lab hi3516ev300 was found sitting on
# 00:00:23:34:45:88 -- a real value in flash, not the fallback -- and sailed
# straight past it. Hence the whole /40 prefix, and hence case 4 below.
#
# Two-part check:
#   Part 1 — behaviour: run the real extutils applets against a stubbed camera
#            and assert what was decided and what reached the store.
#   Part 2 — drift: static assertions the behaviour test cannot pin, chiefly
#            that no shipped file has grown the literal back.
#
# Lightweight: pure shell, no QEMU, no root, runs in a few seconds.

set -u

# Overridable so the suite can be pointed at the comment-stripped build output,
# the way test_sysupgrade.sh is.
SRC=${SRC:-general/overlay/usr/sbin/extutils}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }
T()   { local exp="$1" act="$2" desc="$3"
        if [ "$exp" = "$act" ]; then ok "$desc"; else bad "$desc -- want '$exp', got '$act'"; fi; }

[ -f "$SRC" ] || { echo "FAIL cannot find $SRC — run me from the repo root"; exit 1; }

# ---------------------------------------------------------------------------
# Sandbox. extutils hardcodes three host paths; redirect them under $SB and put
# stubs for everything that would touch real hardware on PATH.
# ---------------------------------------------------------------------------
SB=$(mktemp -d)
trap 'rm -rf "$SB"' EXIT
mkdir -p "$SB/bin" "$SB/etc" "$SB/sys/class/net/eth0"

sed -e "s|^ETHADDR_FILE=/etc/ethaddr|ETHADDR_FILE=$SB/etc/ethaddr|" \
    -e "s|\"/sys/class/net/|\"$SB/sys/class/net/|g" \
    "$SRC" > "$SB/bin/extutils"
chmod +x "$SB/bin/extutils"
grep -q "^ETHADDR_FILE=$SB/etc/ethaddr" "$SB/bin/extutils" ||
    { echo "FAIL sandbox rewrite missed ETHADDR_FILE — the store moved, fix this test"; exit 1; }

# extutils dispatches on $0, so the applets have to be reached by name.
for applet in check_mac get_mac set_mac; do ln -s extutils "$SB/bin/$applet"; done

PATH="$SB/bin:$PATH"

stub() { printf '#!/bin/bash\n%s\n' "$2" > "$SB/bin/$1"; chmod +x "$SB/bin/$1"; }

# The u-boot environment, as a single file the stubs share. STUB_SETENV_RC=1
# models a board whose env partition refuses the write.
UENV=$SB/uboot-env
SETENV_LOG=$SB/setenv.log

stub fw_printenv '
[ "$1" = -n ] && shift
v=$(sed -n "s/^$1=//p" "$UENV" 2>/dev/null)
[ -n "$v" ] || exit 1
echo "$v"'

stub fw_setenv '
echo "$*" >> "$SETENV_LOG"
[ "${STUB_SETENV_RC:-0}" = 0 ] || exit 1
sed -i "/^$1=/d" "$UENV" 2>/dev/null
[ -n "${2:-}" ] && echo "$1=$2" >> "$UENV"
exit 0'

# A reboot is a test failure in itself: #1147 rebooted after writing the MAC,
# and at S30 -- before S40network -- it never had to.
stub reboot 'echo "reboot $*" >> "$SB/reboot.log"; exit 0'

stub ipcinfo '
case "$1" in
    -F)        echo "${STUB_FLASH:-nor}" ;;
    --xm-mac)  [ -n "${STUB_XM_MAC:-}" ] || { echo "Nothing found." >&2; exit 1; }
               echo "$STUB_XM_MAC" ;;
    -v)        echo "${STUB_VENDOR:-hisilicon}" ;;
esac
exit 0'

export UENV SETENV_LOG SB

# Fresh camera: nothing stored anywhere, and a kernel-random MAC on eth0.
KERNEL_MAC=6e:5f:91:50:47:a0
reset_camera() {
    : > "$UENV"; : > "$SETENV_LOG"; rm -f "$SB/etc/ethaddr" "$SB/reboot.log"
    echo "$KERNEL_MAC" > "$SB/sys/class/net/eth0/address"
    unset STUB_XM_MAC STUB_FLASH STUB_SETENV_RC
}

env_mac()    { sed -n 's/^ethaddr=//p' "$UENV"; }
setenv_runs() { [ -s "$SETENV_LOG" ] && wc -l < "$SETENV_LOG" | tr -d ' ' || echo 0; }

# Every repair path must leave the camera with an address it can keep, and must
# leave the same one behind for the next boot.
assert_repaired() {
    local desc="$1" want_prefix="$2"
    local stored; stored=$(get_mac eth0)
    case "$stored" in
        "$want_prefix"*) ok "$desc -- $stored" ;;
        *) bad "$desc -- want an address starting '$want_prefix', got '$stored'" ;;
    esac
    [ "$stored" = "$(env_mac)" ] || bad "$desc -- get_mac '$stored' disagrees with the env '$(env_mac)'"
    [ -f "$SB/reboot.log" ] && bad "$desc -- rebooted; S30 runs before S40network, it must not need to"
    return 0
}

echo "=== Part 1: what check_mac decides ==="

# An address of the camera's own is never touched. This is every camera that is
# already provisioned, i.e. nearly all of them -- a repair that churned these
# would change MACs, and DHCP leases, across the whole fleet on upgrade.
reset_camera
echo "ethaddr=bc:24:11:aa:bb:cc" > "$UENV"
check_mac >/dev/null
T "bc:24:11:aa:bb:cc" "$(env_mac)" "a real vendor MAC is left alone"
T "0" "$(setenv_runs)" "...and nothing is written to the environment"
T "bc:24:11:aa:bb:cc" "$(get_mac eth0)" "...and get_mac reports it"

# The #2405 case: a full image carries no environment.
reset_camera
check_mac >/dev/null
assert_repaired "no ethaddr at all is repaired (#2405)" "02:"
T "1" "$(setenv_runs)" "...with exactly one environment write"

# The placeholder, in both the shape u-boot bakes in and the shape found in the
# lab. :88 is the whole reason this matches a prefix and not a literal.
for ph in 00:00:23:34:45:66 00:00:23:34:45:88 00:00:23:34:45:01 00:00:23:34:45:ff; do
    reset_camera
    echo "ethaddr=$ph" > "$UENV"
    check_mac >/dev/null
    assert_repaired "placeholder $ph is replaced" "02:"
done

# Addresses that are not addresses.
reset_camera; echo "ethaddr=00:00:00:00:00:00" > "$UENV"
check_mac >/dev/null; assert_repaired "the all-zero address is replaced" "02:"

reset_camera; echo "ethaddr=01:02:03:04:05:06" > "$UENV"
check_mac >/dev/null; assert_repaired "a multicast address is replaced (not a legal source)" "02:"

reset_camera; echo "ethaddr=not-a-mac" > "$UENV"
check_mac >/dev/null; assert_repaired "a malformed value is replaced" "02:"

reset_camera; echo "ethaddr=bc:24:11:aa:bb" > "$UENV"
check_mac >/dev/null; assert_repaired "a five-octet value is replaced" "02:"

# A camera converted from stock firmware keeps the address on its own label.
reset_camera
echo "ethaddr=00:00:23:34:45:66" > "$UENV"
STUB_XM_MAC=00:12:16:aa:bb:cc check_mac >/dev/null
T "00:12:16:aa:bb:cc" "$(env_mac)" "the factory MAC in flash wins over a generated one"

# ...but only where that layout can exist, and only if what came back is usable.
reset_camera
STUB_FLASH=nand STUB_XM_MAC=00:12:16:aa:bb:cc check_mac >/dev/null
assert_repaired "the flash scan is skipped on NAND" "02:"

reset_camera
STUB_XM_MAC=00:00:23:34:45:66 check_mac >/dev/null
assert_repaired "a factory MAC that is itself the placeholder is rejected" "02:"

reset_camera
STUB_XM_MAC="Nothing found." check_mac >/dev/null
assert_repaired "junk from the flash scan is rejected" "02:"

echo "=== Part 1b: the address it mints ==="

reset_camera; check_mac >/dev/null; first=$(env_mac)
reset_camera; check_mac >/dev/null; second=$(env_mac)
if [ "$first" != "$second" ]; then ok "two cameras get two addresses ($first / $second)"
else bad "two cameras got the same address $first -- the generator is not random"; fi

# Locally administered (bit 1 set) and unicast (bit 0 clear): 0x02.
octet=$(printf '%d' "0x${first%%:*}")
[ $((octet & 2)) -eq 2 ] && ok "generated address is locally administered" ||
    bad "generated address $first is not locally administered"
[ $((octet & 1)) -eq 0 ] && ok "generated address is unicast" ||
    bad "generated address $first is multicast"

echo "=== Part 2: where it is stored ==="

# A board whose environment refuses the write still has to end up with an
# address -- and the same one next boot.
reset_camera
STUB_SETENV_RC=1 check_mac >/dev/null
stored=$(cat "$SB/etc/ethaddr" 2>/dev/null || echo MISSING)
case "$stored" in
    02:*) ok "a refused environment write falls back to /etc/ethaddr -- $stored" ;;
    *) bad "a refused environment write left nothing usable behind -- got '$stored'" ;;
esac
T "$stored" "$(get_mac eth0)" "...and get_mac prefers it over the kernel's address"

# The two ambarella s2l defconfigs do not select BR2_PACKAGE_UBOOT_TOOLS, so
# there is no fw_printenv or fw_setenv on the image at all.
reset_camera
mv "$SB/bin/fw_printenv" "$SB/fw_printenv.hidden"
mv "$SB/bin/fw_setenv"   "$SB/fw_setenv.hidden"
check_mac >/dev/null
stored=$(cat "$SB/etc/ethaddr" 2>/dev/null || echo MISSING)
case "$stored" in
    02:*) ok "a board with no uboot-tools still gets an address -- $stored" ;;
    *) bad "a board with no uboot-tools got nothing -- '$stored'" ;;
esac
T "$stored" "$(get_mac eth0)" "...and get_mac finds it"
mv "$SB/fw_printenv.hidden" "$SB/bin/fw_printenv"
mv "$SB/fw_setenv.hidden"   "$SB/bin/fw_setenv"

echo "=== Part 3: get_mac never hands ifup an empty value ==="

# interfaces.d/eth0 puts this straight into `hwaddress ether`. busybox ifupdown
# runs the whole thing as one `ip link set addr <mac> eth0 up`; an empty <mac>
# makes that fail, ifupdown treats a failed command as fatal, and udhcpc is
# never started -- the camera boots with no address at all.
reset_camera
T "$KERNEL_MAC" "$(get_mac eth0)" "with nothing stored, get_mac echoes the interface's own address"
[ -n "$(get_mac eth0)" ] || bad "get_mac printed nothing with nothing stored"
[ -n "$(get_mac nosuchif)" ] && bad "get_mac invented an address for an interface that does not exist" ||
    ok "get_mac on an unknown interface prints nothing rather than a made-up address"

echo "=== Part 4: set_mac, the manual override ==="

reset_camera
set_mac bc:24:11:00:00:99 >/dev/null
T "bc:24:11:00:00:99" "$(env_mac)" "set_mac stores the address it is given"

# Unlike check_mac, it replaces a perfectly good address -- being asked is the
# point. This is how two cameras that collided are pulled apart.
before=$(env_mac); set_mac >/dev/null; after=$(env_mac)
[ "$before" != "$after" ] && ok "bare set_mac mints a fresh address over a good one" ||
    bad "bare set_mac left $before in place"

reset_camera
echo "ethaddr=bc:24:11:aa:bb:cc" > "$UENV"
set_mac 00:00:23:34:45:66 >/dev/null 2>&1
T "1" "$?" "set_mac refuses the shared placeholder"
T "bc:24:11:aa:bb:cc" "$(env_mac)" "...and stores nothing"

set_mac zz:zz:zz:zz:zz:zz >/dev/null 2>&1
T "1" "$?" "set_mac refuses a malformed address"
set_mac 01:02:03:04:05:06 >/dev/null 2>&1
T "1" "$?" "set_mac refuses a multicast address"
T "bc:24:11:aa:bb:cc" "$(env_mac)" "...and still stores nothing"

echo "=== Part 5: drift ==="

# The literal must not come back as a fallback anywhere it would be USED.
# Two appearances are legitimate and are filtered out by name, not by luck:
# the comments that explain the whole problem, and the two places that DEFINE
# what the placeholder is (extutils' case pattern, the kernel sentinel).
code_hits=$(grep -rn "00:00:23:34:45" general/overlay general/package 2>/dev/null |
            grep -v "^[^:]*:[0-9]*:[[:space:]]*[#/]" |
            grep -v "00:00:23:34:45:\*)" |
            grep -v "placeholder_mac\[" || true)
if [ -z "$code_hits" ]; then ok "no shipped file falls back to the shared literal"
else bad "the shared literal is back in shipped code:"; printf '%s\n' "$code_hits"; fi

for f in general/overlay/etc/network/interfaces.d/eth0 general/overlay/etc/init.d/S40network; do
    grep -q "get_mac" "$f" && ok "$f asks get_mac for the address" ||
        bad "$f no longer routes through get_mac"
done

# extutils dispatches on $0; an applet with no symlink is dead code, which is
# exactly what set_mac was before this.
for applet in check_mac get_mac set_mac; do
    [ -L "general/overlay/usr/sbin/$applet" ] && ok "$applet is reachable (symlink present)" ||
        bad "general/overlay/usr/sbin/$applet is missing -- the applet is dead code"
done

# Comments stripped first: the arm's own comment explains that it used to
# reboot, and a naive grep reads that as the reboot coming back.
if sed -n '/^\tcheck_mac)/,/^\t;;/p' "$SRC" | grep -v '^[[:space:]]*#' | grep -qw reboot; then
    bad "check_mac reboots again -- S30 runs before S40network, it does not need to"
else
    ok "check_mac does not reboot"
fi

echo
[ "$fail" -eq 0 ] && { echo "All checks passed."; exit 0; }
echo "$fail check(s) failed."; exit 1
