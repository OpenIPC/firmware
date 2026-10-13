#!/bin/bash
# Regression tests for cable-primary networking: S40network's dual mode and its
# carrier watcher, and default.script's route metrics and merged resolv.conf.
#
# Everything these get wrong is a camera that cannot be reached: an eth0
# parked on a static address while a cable is plugged in, a wlan0 route that
# takes the cable's traffic, a watcher that releases the lease on a carrier
# blip -- or on a link someone brought down by hand -- or one that kills a
# stranger's process through a stale pidfile. None of it shows on a bench
# without pulling cables, and the shared overlay ships it to every camera.
#
# Runs the real scripts in a sandbox: their device paths are rewritten under
# $SB, the watcher's poll is shortened, and the network tools are stubs that
# log what they were asked to do. The udhcpc stub is a real process whose comm
# is "udhcpc", so the watcher's signals land on it as they would on a camera.
#
# Lightweight: pure shell, no root, runs in well under a minute.

set -u

S40=${S40:-general/overlay/etc/init.d/S40network}
DS=${DS:-general/overlay/usr/share/udhcpc/default.script}
# Not busybox sh: a busybox built to prefer its applets runs its own ifup and
# ip over the stubs below. The scripts' busybox-ash parse is
# test_shell_parse.sh's job.
SHELL_UNDER_TEST=${SHELL_UNDER_TEST:-dash}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

for f in "$S40" "$DS"; do
    [ -f "$f" ] || { echo "FAIL cannot find $f — run me from the repo root"; exit 1; }
done

SB=$(mktemp -d)
cleanup() {
    pkill -f "$SB/" 2>/dev/null
    rm -rf "$SB"
}
trap cleanup EXIT
mkdir -p "$SB/bin" "$SB/run" "$SB/tmp" "$SB/env" "$SB/wireless" \
    "$SB/sys/class/net/eth0" "$SB/proc/sys/net/ipv4/conf/eth0" \
    "$SB/proc/sys/net/ipv4/conf/wlan0"

# The scripts with their paths moved into the sandbox. /proc/$pid is left
# alone: the watcher and the udhcpc stub are real processes.
rewrite() {
    sed -e "s|/sbin/ifconfig|ifconfig|g" \
        -e "s|/tmp/|$SB/tmp/|g" \
        -e "s|/var/run/|$SB/run/|g" \
        -e "s|/sys/class/net|$SB/sys/class/net|g" \
        -e "s|/proc/sys/net|$SB/proc/sys/net|g" \
        -e "s|/etc/wireless|$SB/wireless|g" \
        -e "s|/usr/share/udhcpc/default.script|$SB/default.script|g" \
        -e "s|^POLL=1$|POLL=0.1|" \
        -e "s|^DOWN_AFTER=2$|DOWN_AFTER=10|" \
        -e "s|^SETTLE=5$|SETTLE=30|" \
        "$1" > "$2"
    chmod +x "$2"
}
# The watcher polls every 0.1 s here, releases after DOWN_AFTER polls (1 s) and
# after SETTLE polls (3 s) once eth0 is brought up by hand. The carrier changes
# below sit well clear of both: a shared CI runner can stall this shell for a
# good fraction of a second, so a gap under ~0.5 s either way makes a case
# that passes or fails by load alone.
rewrite "$S40" "$SB/S40network"
rewrite "$DS" "$SB/default.script"
for p in '^POLL=1$' '^DOWN_AFTER=2$' '^SETTLE=5$'; do
    grep -q "$p" "$S40" || bad "$p is gone from S40network; the watcher tests run at full speed"
done

LOG="$SB/log"
stub() { printf '#!/bin/sh\n%s\n' "$2" > "$SB/bin/$1"; chmod +x "$SB/bin/$1"; }
# fw_printenv -n NAME: the value in $SB/env/NAME, or the error a missing
# variable gives.
stub fw_printenv '[ "$1" = -n ] && shift
[ -f "'"$SB"'/env/$1" ] && { cat "'"$SB"'/env/$1"; exit 0; }
echo "## Error: \"$1\" not defined" >&2; exit 1'
stub get_mac 'echo 00:11:22:33:44:55'
# arping: the gateway answers unless $SB/no-arp says it does not.
stub arping 'echo "arping $*" >> "'"$LOG"'"; [ ! -e "'"$SB"'/no-arp" ]'
stub logger 'echo "logger $*" >> "'"$LOG"'"'
stub ifconfig 'echo "ifconfig $*" >> "'"$LOG"'"'
# route del fails, as it does once there is nothing left to delete: the
# script deletes in a loop until it does.
stub route 'echo "route $*" >> "'"$LOG"'"; [ "$1" != del ]'
stub ifdown 'echo "ifdown $*" >> "'"$LOG"'"'
stub timeout 'exit 0'
# ifup eth0 starts a udhcpc, as busybox ifupdown does -- unless $SB/static
# says eth0's stanza is a static one.
stub ifup 'echo "ifup $*" >> "'"$LOG"'"
[ "$1" = eth0 ] && [ ! -e "'"$SB"'/static" ] && { "'"$SB"'/bin/udhcpc" & echo $! > "'"$SB"'/run/udhcpc.eth0.pid"; }
exit 0'
# udhcpc: records the signals it gets.
stub udhcpc 'trap "echo USR1 >> '"$SB"'/udhcpc.sig" USR1
trap "echo USR2 >> '"$SB"'/udhcpc.sig" USR2
while :; do sleep 0.05; done'
# ip: route listings come from $SB/routes.<dev>; everything else is logged.
# A full "ip -4|-6 route show" comes from $SB/routes.all4 / routes.all6.
stub ip 'echo "ip $*" >> "'"$LOG"'"
if [ "$2" = route ] && [ "$3" = show ]; then
    if [ "$4" = dev ]; then cat "'"$SB"'/routes.$5" 2>/dev/null
    else cat "'"$SB"'/routes.all${1#-}" 2>/dev/null; fi
fi
exit 0'
# /etc/wireless: "good-usb" loads through usb, "good-modem" through modem,
# anything else fails, as a missing driver does.
for k in usb sdio modem; do
    printf '#!/bin/sh\n[ "$1" = "good-%s" ] && exit 0\nexit 1\n' "$k" > "$SB/wireless/$k"
    chmod +x "$SB/wireless/$k"
done

export PATH="$SB/bin:$PATH"

setenv() { echo "$2" > "$SB/env/$1"; }
link() {   # link <flags> <carrier>
    echo "$1" > "$SB/sys/class/net/eth0/flags"
    echo "$2" > "$SB/sys/class/net/eth0/carrier"
}
reset() {
    $SHELL_UNDER_TEST "$SB/S40network" stop >/dev/null 2>&1
    pkill -f "$SB/bin/udhcpc" 2>/dev/null
    rm -rf "$SB/env"/* "$SB/run"/* "$SB/tmp"/* "$SB/udhcpc.sig" "$SB"/routes.* "$SB/static" "$SB/no-arp"
    rm -f "$SB/proc/sys/net/ipv4/conf"/*/arp_*
    for i in eth0 wlan0; do
        : > "$SB/proc/sys/net/ipv4/conf/$i/arp_ignore"
        : > "$SB/proc/sys/net/ipv4/conf/$i/arp_announce"
    done
    link 0x1003 0
    : > "$LOG"
}
logged()  { grep -qxF -- "$1" "$LOG"; }
count()   { grep -cxF -- "$1" "$LOG"; }
sigs()    { tr '\n' ' ' 2>/dev/null < "$SB/udhcpc.sig"; }
got()     { grep -q "$1" "$SB/udhcpc.sig" 2>/dev/null; }
watcher() { pgrep -f "$SB/S40network watch" | head -1; }
watching() { [ -n "$(watcher)" ]; }
start()   { $SHELL_UNDER_TEST "$SB/S40network" start >/dev/null 2>&1; }
# wait_for <seconds> <command...>: poll until the command succeeds.
wait_for() {
    local t=$(( $1 * 10 )); shift
    while [ $t -gt 0 ]; do "$@" && return 0; sleep 0.1; t=$((t - 1)); done
    return 1
}

echo "=== S40network: the modes that must not change ==="

reset
start
if logged "ifup eth0" && [ -z "$(watcher)" ] && [ ! -e "$SB/run/network.fallback" ]; then
    ok "no wlandev: ifup eth0, no watcher"
else
    bad "no wlandev, log='$(cat "$LOG")'"
fi

reset; setenv wlandev good-usb
start
if logged "ifup eth0" && ! logged "ifup wlan0" && [ -z "$(watcher)" ]; then
    ok "wlandev without wlanssid: eth0 DHCP, wlan0 left to qrscan (#2572)"
else
    bad "wlandev without wlanssid, log='$(cat "$LOG")'"
fi

reset; setenv wlandev good-usb; setenv wlanssid Home; setenv netmode wifi
start
if logged "ifup wlan0" && logged "ifconfig eth0 192.168.2.10 hw ether 00:11:22:33:44:55 up" &&
    ! logged "ifup eth0" && [ -z "$(watcher)" ]; then
    ok "netmode=wifi: today's layout -- wlan0 DHCP, eth0 static, no watcher"
else
    bad "netmode=wifi, log='$(cat "$LOG")'"
fi

reset; setenv wlandev good-modem
start
if logged "ifup usb0" && logged "ifup eth1" && logged "ifconfig eth0 192.168.2.10 hw ether 00:11:22:33:44:55 up" &&
    [ -z "$(watcher)" ]; then
    ok "modem: usb0 and eth1, eth0 static, no watcher"
else
    bad "modem, log='$(cat "$LOG")'"
fi

echo "=== S40network: dual mode ==="

reset; setenv wlandev good-usb; setenv wlanssid Home; setenv netaddr_fallback 10.9.8.7
start
if logged "ifup wlan0" && logged "ifconfig eth0 hw ether 00:11:22:33:44:55 up" && ! logged "ifup eth0" &&
    [ "$(cat "$SB/run/network.fallback" 2>/dev/null)" = 10.9.8.7 ] &&
    [ "$(cat "$SB/proc/sys/net/ipv4/conf/eth0/arp_ignore")" = 1 ] &&
    [ "$(cat "$SB/proc/sys/net/ipv4/conf/wlan0/arp_announce")" = 2 ] &&
    wait_for 2 watching; then
    ok "dual: wlan0 joins, eth0 link up with no address, fallback and ARP set, watcher running"
else
    bad "dual start, log='$(cat "$LOG")'"
fi

# No cable: the watcher waits, and S40network has started no DHCP on eth0.
sleep 0.5
if ! logged "ifup eth0"; then
    ok "dual, no cable: no DHCP on eth0"
else
    bad "dual started eth0's DHCP with no cable, log='$(cat "$LOG")'"
fi

# Cable in: one ifup eth0.
link 0x1003 1
if wait_for 3 logged "ifup eth0" && sleep 0.5 && [ "$(count "ifup eth0")" = 1 ]; then
    ok "cable plugged in: the watcher starts eth0's DHCP, once"
else
    bad "ifup eth0 count=$(count "ifup eth0"), log='$(cat "$LOG")'"
fi

# A carrier blip shorter than DOWN_AFTER polls: nothing.
sleep 0.3
link 0x1003 0; sleep 0.15; link 0x1003 1; sleep 1.5
if [ -z "$(sigs)" ]; then
    ok "a one-poll carrier blip releases nothing"
else
    bad "a blip was acted on: signals='$(sigs)'"
fi

# Cable pulled: USR2 (release). Back: USR1 (renew).
link 0x1003 0
if wait_for 3 got USR2; then
    ok "cable pulled: eth0's lease is released"
else
    bad "no release after the cable went, signals='$(sigs)'"
fi
link 0x1003 1
if wait_for 3 got USR1 && [ "$(sigs)" = "USR2 USR1 " ]; then
    ok "cable back: eth0's lease is renewed"
else
    bad "after replug, signals='$(sigs)'"
fi
if grep -q "logger -t network eth0 link lost" "$LOG" && grep -q "logger -t network eth0 link back" "$LOG"; then
    ok "both transitions are logged"
else
    bad "missing log lines, log='$(cat "$LOG")'"
fi

# Brought down by hand: the kernel flushed the routes already, nothing to do.
: > "$SB/udhcpc.sig"
link 0x1002 0; sleep 1
if [ -z "$(sigs)" ]; then
    ok "eth0 brought down by hand: no signal"
else
    bad "admin down was signalled: '$(sigs)'"
fi
link 0x1003 1
if wait_for 3 got USR1; then
    ok "...and brought back up with a cable: renewed"
else
    bad "no renew after admin up, signals='$(sigs)'"
fi

# Brought back up by hand, autonegotiation outlasting DOWN_AFTER polls -- as
# measured on an hi3516ev300 -- is not a pulled cable: no release, and the
# renew that puts back the routes the kernel flushed.
: > "$SB/udhcpc.sig"
link 0x1002 0; sleep 0.5
link 0x1003 0; sleep 1.5
link 0x1003 1
if wait_for 3 got USR1 && [ "$(sigs)" = "USR1 " ]; then
    ok "admin up with slow autonegotiation: renewed, never released"
else
    bad "admin up with slow autoneg, signals='$(sigs)'"
fi

# ...but brought up with no cable at all, the lease goes once SETTLE runs out.
: > "$SB/udhcpc.sig"
link 0x1002 0; sleep 0.5
link 0x1003 0
if wait_for 6 got USR2; then
    ok "admin up with no cable: released after the settle time"
else
    bad "admin up with no cable, signals='$(sigs)'"
fi
link 0x1003 1
wait_for 3 got USR1 || bad "no renew when the cable came back after that"

# One watcher however often start runs, none after stop.
start; start; sleep 0.3
n=$(pgrep -fc "$SB/S40network watch")
if [ "$n" = 1 ]; then
    ok "start twice: one watcher"
else
    bad "start twice left $n watchers"
fi
$SHELL_UNDER_TEST "$SB/S40network" stop >/dev/null 2>&1
if [ -z "$(watcher)" ] && [ ! -e "$SB/run/network-watch.pid" ] && [ ! -e "$SB/run/network.fallback" ]; then
    ok "stop: no watcher, no pidfile, no fallback marker"
else
    bad "stop left watcher=$(watcher) pidfile=$(cat "$SB/run/network-watch.pid" 2>/dev/null)"
fi

# A stale pidfile naming somebody else's process: it survives stop.
reset
sleep 30 &
stranger=$!
echo "$stranger" > "$SB/run/network-watch.pid"
$SHELL_UNDER_TEST "$SB/S40network" stop >/dev/null 2>&1
if kill -0 "$stranger" 2>/dev/null; then
    ok "a stale pidfile does not get an unrelated process killed"
else
    bad "stop killed the process a stale pidfile named"
fi
kill "$stranger" 2>/dev/null

# A static eth0 stanza has no udhcpc to signal. Its routes -- IPv4 and IPv6 --
# are put aside and flushed when the cable goes, so a gateway route cannot
# outlive the cable, and put back when it returns: no re-apply, no bounce of a
# link that has just come back, no traffic lost on the way back.
reset; setenv wlandev good-usb; setenv wlanssid Home
touch "$SB/static"
printf 'default via 10.216.128.1 dev eth0\n10.216.128.0/24 dev eth0 scope link  src 10.216.128.87\n10.216.128.0/24 dev wlan0 scope link  src 10.216.128.70  metric 10\n' > "$SB/routes.all4"
printf '2001:db8::/64 dev eth0  metric 256\ndefault via 2001:db8::1 dev eth0  metric 1024\n' > "$SB/routes.all6"
link 0x1003 1
start
wait_for 3 logged "ifup eth0"
sleep 0.5
link 0x1003 0
# The IPv6 flush follows the IPv4 one, so wait for both.
both_flushed() { logged "ip -4 route flush dev eth0" && logged "ip -6 route flush dev eth0"; }
if wait_for 3 both_flushed && ! grep -q "addr flush" "$LOG" && [ -z "$(sigs)" ]; then
    ok "static eth0, cable pulled: its IPv4 and IPv6 routes are flushed, the address kept"
else
    bad "static release, log='$(cat "$LOG")' signals='$(sigs)'"
fi
# Carrier back, but the gateway not answering yet -- a switch port still
# learning: the routes stay out, and WiFi keeps the traffic.
touch "$SB/no-arp"
link 0x1003 1
sleep 1
if grep -q "^arping .*-I eth0 10.216.128.1" "$LOG" && ! grep -q "^ip -4 route add" "$LOG"; then
    ok "static eth0, carrier back before the gateway answers: its routes stay out"
else
    bad "routes restored before the gateway answered, log='$(cat "$LOG")'"
fi
rm -f "$SB/no-arp"
# IPv6 routes come back after the IPv4 ones, so wait for the last of them.
all_back() {
    logged "ip -4 route add default via 10.216.128.1 dev eth0" &&
        logged "ip -6 route add default via 2001:db8::1 dev eth0 metric 1024"
}
if wait_for 3 all_back; then
    a=$(grep -n "^ip -4 route add 10.216.128.0/24 dev eth0" "$LOG" | cut -d: -f1)
    d=$(grep -n "^ip -4 route add default via 10.216.128.1 dev eth0" "$LOG" | cut -d: -f1)
    if [ -n "$a" ] && [ "$a" -lt "$d" ] && ! grep -q "dev wlan0" <(grep "route add" "$LOG") &&
        logged "ip -6 route add default via 2001:db8::1 dev eth0 metric 1024"; then
        ok "static eth0, cable back: its routes come back, the subnet before the gateway, IPv6 too"
    else
        bad "static restore order, log='$(cat "$LOG")'"
    fi
else
    bad "static restore, log='$(cat "$LOG")'"
fi
if [ "$(count "ifup eth0")" = 1 ] && ! grep -q "^ifdown" "$LOG"; then
    ok "static eth0, cable back: nothing re-applied, so the link is not bounced"
else
    bad "the static restore re-applied the stanza, log='$(cat "$LOG")'"
fi

# Brought down by hand there is nothing put aside -- the kernel flushed the
# routes itself -- so the stanza is applied again.
link 0x1002 0; sleep 0.5; link 0x1003 1
if wait_for 3 logged "ifdown -f eth0"; then
    ok "static eth0 brought down and up by hand: the stanza is applied again"
else
    bad "admin bounce of a static eth0, log='$(cat "$LOG")'"
fi

# A lease's DNS on file with no udhcpc left to release it -- the client died
# after it bound: released as udhcpc would have, so the cable's servers do not
# sit ahead of the WiFi's.
reset; setenv wlandev good-usb; setenv wlanssid Home
touch "$SB/static"
mkdir -p "$SB/tmp/resolv.d"
printf 'nameserver 10.216.128.1\n' > "$SB/tmp/resolv.d/00000.eth0"
printf 'nameserver 192.168.1.1\n' > "$SB/tmp/resolv.d/00010.wlan0"
link 0x1003 1
start
wait_for 3 logged "ifup eth0"
sleep 0.5
link 0x1003 0
# deconfig logs the ifconfig before it rewrites resolv.conf, so wait for the
# whole end state, not just the log line.
stale_released() {
    logged "ifconfig eth0 0.0.0.0" && [ ! -e "$SB/tmp/resolv.d/00000.eth0" ] &&
        [ "$(cat "$SB/tmp/resolv.conf" 2>/dev/null)" = "nameserver 192.168.1.1" ]
}
if wait_for 3 stale_released; then
    ok "a dead udhcpc's lease on a pulled cable: released, its DNS gone from resolv.conf"
else
    bad "stale lease, log='$(cat "$LOG")' resolv='$(cat "$SB/tmp/resolv.conf" 2>/dev/null)'"
fi

# WiFi asked for, its driver missing: the cable gets DHCP, not a static address.
reset; setenv wlandev no-such-driver; setenv wlanssid Home
link 0x1003 1
start
if wait_for 3 logged "ifup eth0" && ! logged "ifconfig eth0 192.168.2.10 hw ether 00:11:22:33:44:55 up" &&
    ! logged "ifup wlan0"; then
    ok "WiFi driver missing: eth0 gets DHCP"
else
    bad "missing driver, log='$(cat "$LOG")'"
fi

echo "=== default.script ==="

ds() {   # ds <action> <interface> [VAR=value ...]
    local action=$1 iface=$2; shift 2
    env interface="$iface" "$@" $SHELL_UNDER_TEST "$SB/default.script" "$action" >/dev/null 2>&1
}
resolv() { cat "$SB/tmp/resolv.conf" 2>/dev/null; }

# Outside dual mode -- netmode=wifi, the modem layout -- eth0 sits on a static
# fallback whose connected route is metric 0. A wlan0 demoted beside it would
# lose its own subnet to a dead port whenever the WiFi network used that
# subnet, so there wlan0 keeps the metric-0 routes it always had.
reset
printf '192.168.2.0/24 scope link  src 192.168.2.50\n' > "$SB/routes.wlan0"
ds bound wlan0 ip=192.168.2.50 subnet=255.255.255.0 router=192.168.2.1 dns="192.168.2.1"
if logged "route add default gw 192.168.2.1 dev wlan0" && ! grep -q "^ip route del" "$LOG"; then
    ok "outside dual mode wlan0 keeps metric 0 and its connected route"
else
    bad "wlan0 outside dual mode, log='$(cat "$LOG")'"
fi

# Dual mode from here on: S40network's marker is what default.script keys on.
reset
echo 192.168.2.10 > "$SB/run/network.fallback"
ds bound eth0 ip=10.0.0.5 subnet=255.255.255.0 router=10.0.0.1 dns="10.0.0.1 1.1.1.1" domain=lan
if logged "route add default gw 10.0.0.1 dev eth0" && ! grep -q "^ip route del" "$LOG" &&
    [ "$(resolv)" = "$(printf 'search lan\nnameserver 10.0.0.1\nnameserver 1.1.1.1')" ]; then
    ok "eth0 bound: metric 0 default route, connected route untouched, resolv.conf as before"
else
    bad "eth0 bound, log='$(cat "$LOG")' resolv='$(resolv)'"
fi

: > "$LOG"
printf '192.168.1.0/24 scope link  src 192.168.1.50\n' > "$SB/routes.wlan0"
ds bound wlan0 ip=192.168.1.50 subnet=255.255.255.0 router=192.168.1.1 dns="192.168.1.1" domain=home
if logged "route add default gw 192.168.1.1 dev wlan0 metric 10" &&
    logged "ip route del 192.168.1.0/24 dev wlan0 proto kernel" &&
    logged "ip route add 192.168.1.0/24 dev wlan0 proto kernel scope link src 192.168.1.50 metric 10"; then
    ok "wlan0 bound: default and connected routes at metric 10"
else
    bad "wlan0 bound, log='$(cat "$LOG")'"
fi
if [ "$(resolv)" = "$(printf 'search lan\nnameserver 10.0.0.1\nnameserver 1.1.1.1\nnameserver 192.168.1.1')" ]; then
    ok "resolv.conf: the cable's servers and search first, then WiFi's"
else
    bad "merged resolv.conf='$(resolv)'"
fi

: > "$LOG"
printf '192.168.1.0/24 scope link  src 192.168.1.50  metric 10\n' > "$SB/routes.wlan0"
ds renew wlan0 ip=192.168.1.50 subnet=255.255.255.0 router=192.168.1.1 dns="192.168.1.1"
if ! grep -q "^ip route del" "$LOG"; then
    ok "wlan0 renew: a connected route already at its metric is left alone"
else
    bad "renew rewrote the connected route again, log='$(cat "$LOG")'"
fi

ds deconfig eth0
if [ "$(resolv)" = "$(printf 'nameserver 192.168.1.1')" ]; then
    ok "eth0 deconfig: only WiFi's DNS is left"
else
    bad "after eth0 deconfig resolv='$(resolv)'"
fi

reset
printf 'nameserver 9.9.9.9\n' > "$SB/tmp/resolv.conf"
ds deconfig wlan0
if [ "$(resolv)" = "nameserver 9.9.9.9" ]; then
    ok "deconfig with no fragment of its own leaves resolv.conf as it was"
else
    bad "a first deconfig rewrote resolv.conf: '$(resolv)'"
fi

reset
ds bound wlan0 ip=192.168.1.50 router=192.168.1.1 IF_METRIC=20
ds bound eth1 ip=172.16.0.5 router=172.16.0.1 IF_METRIC=junk
if logged "route add default gw 192.168.1.1 dev wlan0 metric 20" && logged "route add default gw 172.16.0.1 dev eth1"; then
    ok "IF_METRIC (a stanza's metric line) overrides; a non-number is ignored"
else
    bad "IF_METRIC, log='$(cat "$LOG")'"
fi

reset
echo 10.9.8.7 > "$SB/run/network.fallback"
link 0x1003 1
ds leasefail eth0
link 0x1003 0
ds leasefail eth0
if logged "ifconfig eth0 10.9.8.7 up" && logged "ifconfig eth0 0.0.0.0" && ! grep -q "192.168.1.10" "$LOG"; then
    ok "dual eth0 leasefail: the fallback with a cable, nothing without, never 192.168.1.10"
else
    bad "dual leasefail, log='$(cat "$LOG")'"
fi

reset
ds leasefail eth0
# The line itself is unchanged; dash's echo prints the -n it is given.
if grep -q "^ifconfig eth0 .*192.168.1.10 .*255.255.255.0 up$" "$LOG"; then
    ok "leasefail outside dual mode: ipaddr's default, as before"
else
    bad "plain leasefail, log='$(cat "$LOG")'"
fi

echo
if [ "$fail" -eq 0 ]; then
    echo "All network failover checks passed."
else
    echo "$fail check(s) failed."
    exit 1
fi
