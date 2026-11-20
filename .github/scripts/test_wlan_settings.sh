#!/bin/bash
# Regression tests for where the Wi-Fi settings live (wlan_settings).
#
# Default boards keep wlandev/wlanssid/wlanpass in the U-Boot environment,
# exactly as before. A board with /etc/default/wlan WLAN_STORE=overlay keeps
# the network in /etc/wlan.conf and must never write the
# environment. In both, the key must not appear on any command line.
#
# Pure shell with stubs; no root, no hardware.

set -u
SRC_SETTINGS=${SRC_SETTINGS:-general/overlay/usr/sbin/wlan_settings}
SRC_CONF=${SRC_CONF:-general/overlay/usr/sbin/wlan_conf}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }
T()   { if [ "$1" = "$2" ]; then ok "$3"; else bad "$3 -- want '$1', got '$2'"; fi; }

for f in "$SRC_SETTINGS" "$SRC_CONF"; do
	[ -f "$f" ] || { echo "FAIL cannot find $f -- run me from the repo root"; exit 1; }
done

SB=$(mktemp -d)
trap 'rm -rf "$SB"' EXIT
mkdir -p "$SB/bin" "$SB/etc/default"
sed -e "s|/etc/default/wlan|$SB/etc/default/wlan|g" \
    -e "s|^CONF=/etc/wlan.conf|CONF=$SB/etc/wlan.conf|" \
    "$SRC_SETTINGS" > "$SB/bin/wlan_settings"
cp "$SRC_CONF" "$SB/bin/wlan_conf"
chmod +x "$SB/bin/wlan_settings" "$SB/bin/wlan_conf"
grep -q "CONF=$SB/etc/wlan.conf" "$SB/bin/wlan_settings" ||
	{ echo "FAIL sandbox rewrite missed CONF -- fix this test"; exit 1; }

UENV=$SB/uboot-env
ARGV_LOG=$SB/argv.log
export UENV ARGV_LOG
stub() { printf '#!/bin/bash\necho "$0 $*" >> "$ARGV_LOG"\n%s\n' "$2" > "$SB/bin/$1"; chmod +x "$SB/bin/$1"; }
stub fw_printenv '[ "$1" = -n ] && shift; v=$(sed -n "s/^$1=//p" "$UENV" 2>/dev/null); [ -n "$v" ] || exit 1; echo "$v"'
stub fw_setenv 'sed -i "/^$1=/d" "$UENV"; echo "$1=$2" >> "$UENV"'
# A stand-in for wpa_passphrase: the passphrase must come on stdin.
stub wpa_passphrase 'read -r p; echo "network={"; echo "	#psk=\"$p\""; echo "	psk=$(printf %s "$1$p" | sha256sum | cut -c1-64)"; echo "}"'
export PATH="$SB/bin:$PATH"

PASS='s3cret passphrase'
reset() { : > "$UENV"; : > "$ARGV_LOG"; rm -f "$SB/etc/default/wlan" "$SB/etc/wlan.conf"; }

echo "=== environment store (default) ==="
reset
printf 'wlandev=rtl8188fu-generic\n' > "$UENV"
T "rtl8188fu-generic" "$(wlan_settings dev)" "dev comes from the environment"
wlan_settings configured && bad "nothing saved yet, yet configured" || ok "not configured before a save"
printf 'HomeNet\n%s\n' "$PASS" | wlan_settings save
T "HomeNet" "$(sed -n 's/^wlanssid=//p' "$UENV")" "save writes wlanssid"
wlan_settings configured && ok "configured after a save" || bad "not configured after a save"
wlan_settings conf | grep -q 'ssid=486f6d654e6574' && ok "conf names the network in hex" || bad "conf lacks the hex SSID"
[ ! -e "$SB/etc/wlan.conf" ] && ok "no overlay file in environment mode" || bad "overlay file written in environment mode"

echo "=== overlay store (WLAN_STORE=overlay) ==="
reset
# wpa_supplicant's package stub at /etc/wpa_supplicant.conf is not a network
printf 'ctrl_interface=/var/run/wpa_supplicant\nnetwork={\n key_mgmt=NONE\n}\n' > "$SB/etc/wpa_supplicant.conf"
printf 'WLAN_STORE=overlay\nWLANDEV=rtl8188fu-generic\n' > "$SB/etc/default/wlan"
T "rtl8188fu-generic" "$(wlan_settings dev)" "dev falls back to WLANDEV"
wlan_settings configured && bad "configured with no file" || ok "not configured before a save"
printf 'HomeNet\n%s\n' "$PASS" | wlan_settings save
T "" "$(cat "$UENV")" "save writes nothing to the environment"
T "600" "$(stat -c %a "$SB/etc/wlan.conf")" "the saved file is root-only"
wlan_settings configured && ok "configured after a save" || bad "not configured after a save"
wlan_settings conf | grep -q 'ssid=486f6d654e6574' && ok "conf returns the saved network" || bad "conf lacks the network"
printf 'wlandev=other-dev\n' > "$UENV"
T "other-dev" "$(wlan_settings dev)" "an environment wlandev still wins"

echo "=== keys never on a command line ==="
grep -qF "$PASS" "$ARGV_LOG" && bad "the passphrase appeared in argv: $(grep -F "$PASS" "$ARGV_LOG" | head -1)" ||
	ok "the passphrase never appeared in any argv"
reset
printf 'WLAN_STORE=overlay\n' > "$SB/etc/default/wlan"
printf 'OpenNet\n\n' | wlan_settings save
wlan_settings conf | grep -q 'key_mgmt=NONE' && ok "an empty key saves an open network" || bad "open network not saved"
HEX=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
printf 'PskNet\n%s\n' "$HEX" | wlan_settings save
wlan_settings conf | grep -q "psk=$HEX" && ok "a raw 64-hex PSK is kept as is" || bad "raw PSK not kept"

echo "=== callers ==="
grep -q 'wlan_settings conf' general/overlay/etc/network/interfaces.d/wlan0 && ok "interfaces.d/wlan0 joins through wlan_settings" ||
	bad "interfaces.d/wlan0 does not use wlan_settings"
grep -q 'fw_printenv -n wlanpass\|fw_setenv wlan' general/overlay/etc/network/interfaces.d/wlan0 general/package/quirc-openipc/files/qrscan.sh &&
	bad "a caller still reads or writes the Wi-Fi environment directly" || ok "no caller touches the Wi-Fi environment directly"

echo
if [ $fail -eq 0 ]; then echo "All checks passed."; else echo "$fail check(s) failed."; exit 1; fi
