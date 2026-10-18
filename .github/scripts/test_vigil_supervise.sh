#!/bin/bash
# Regression tests for S95vigil: the supervisor it shares with S95majestic
# (restarted after a crash, never in a loop, never during an upgrade, never
# after a stop), and what is its own -- the arguments built from
# /etc/vigil.yaml, and the recording directory it refuses.
#
# The same sandbox as test_majestic_supervise.sh: the real script with its
# paths rewritten under $SB and its waits shortened; vigild, start-stop-daemon,
# yaml-cli and logger are stubs, and /proc/mounts is a file the test writes.
#
# Lightweight: pure shell, no root, runs in a few seconds.

set -u

SRC=${SRC:-general/package/vigil/files/S95vigil}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

[ -f "$SRC" ] || { echo "FAIL cannot find $SRC — run me from the repo root"; exit 1; }

SB=$(mktemp -d)
trap 'pkill -f "$SB/" 2>/dev/null; rm -rf "$SB"' EXIT
mkdir -p "$SB/bin" "$SB/run" "$SB/crash" "$SB/cfg" "$SB/disk" "$SB/ram" "$SB/proc/self"

sed -e "s|DAEMON_PATH=\"/usr/bin/\$DAEMON\"|DAEMON_PATH=\"$SB/bin/vigild\"|" \
    -e "s|/var/run/|$SB/run/|g" \
    -e "s|CRASH=/etc/crash|CRASH=$SB/crash|" \
    -e "s|UPGRADE_LOCK=/tmp/sysupgrade.lock|UPGRADE_LOCK=$SB/sysupgrade.lock|" \
    -e "s|BACKOFF=\"5 30 120\"|BACKOFF=\"\${TEST_BACKOFF:-0 0 0}\"|" \
    -e "s|/proc/mounts|$SB/mounts|" \
    -e "s|CARD_POLL=10|CARD_POLL=1|" \
    "$SRC" > "$SB/S95vigil"
for p in 'DAEMON_PATH="/usr/bin/$DAEMON"' /var/run/ CRASH=/etc/crash UPGRADE_LOCK=/tmp/sysupgrade.lock 'BACKOFF="5 30 120"' /proc/mounts CARD_POLL=10; do
    grep -qF "$p" "$SRC" || bad "$p is gone from S95vigil; this test is testing nothing there"
done
chmod +x "$SB/S95vigil"

# A disk, a RAM disk and the flash root, as /proc/mounts would list them.
# A mount point with a space in it is listed with the space escaped (\040).
mkdir -p "$SB/Disk One"
cat > "$SB/mounts" <<EOF
/dev/root / squashfs ro 0 0
tmpfs $SB/ram tmpfs rw 0 0
/dev/sda1 $SB/disk ext4 rw 0 0
/dev/sdb1 $SB/Disk\\040One ext4 rw 0 0
proc $SB/proc proc rw 0 0
EOF
cp "$SB/mounts" "$SB/mounts.boot"

# vigild: counts its starts, keeps its arguments one per line, then does what
# $SB/mode says.
cat > "$SB/bin/vigild" <<EOF
#!/bin/sh
echo start >> "$SB/starts"
: > "$SB/args"
for a in "\$@"; do printf '%s\n' "\$a" >> "$SB/args"; done
case "\$(cat "$SB/mode")" in
    segv) kill -SEGV \$\$ ;;
    term) kill -TERM \$\$ ;;
    exit) exit 0 ;;
    noso) exit 127 ;;
    run) exec sleep 1000 ;;
esac
EOF
cat > "$SB/bin/start-stop-daemon" <<'EOF'
#!/bin/sh
mode= test= sig=TERM exe=
while [ $# -gt 0 ]; do
    case "$1" in
        -S) mode=start ;; -K) mode=stop ;; -t) test=1 ;; -q) ;;
        -s) shift; sig=$1 ;; -x) shift; exe=$1 ;; -a) shift ;;
        --) shift; break ;;
    esac
    shift
done
case "$mode" in
    start) exec "$exe" "$@" ;;
    stop)
        pids=$(pgrep -f "^/bin/sh $exe|^sleep 1000" 2>/dev/null)
        [ -n "$pids" ] || exit 1
        [ -n "$test" ] && exit 0
        kill -s "$sig" $pids 2>/dev/null
        exit 0 ;;
esac
EOF
# yaml-cli -i FILE -g .a.b: the value is the file $SB/cfg/.a.b, absent = not set;
# with $SB/cfg/.broken present it fails to parse, as the real one does.
cat > "$SB/bin/yaml-cli" <<EOF
#!/bin/sh
if [ -f "$SB/cfg/.broken" ]; then
    echo "Parse error: while parsing a block collection at line 2, column 3" >&2
    exit 1
fi
[ "\$3" = -g ] && [ -f "$SB/cfg/\$4" ] && cat "$SB/cfg/\$4"
EOF
printf '#!/bin/sh\necho "$*" >> "%s/log"\n' "$SB" > "$SB/bin/logger"
chmod +x "$SB/bin"/*
export PATH="$SB/bin:$PATH"

starts() { [ -f "$SB/starts" ] && wc -l < "$SB/starts" | tr -d ' ' || echo 0; }
reset() {
    rm -f "$SB/starts" "$SB/args" "$SB/log" "$SB/crash"/* "$SB/sysupgrade.lock" "$SB/run"/* "$SB/cfg"/.*[a-z]* 2>/dev/null
    echo "$1" > "$SB/mode"
}
supervise() { timeout 20 sh "$SB/S95vigil" supervise; }
args() { tr '\n' ' ' < "$SB/args" 2>/dev/null | sed 's/ $//'; }

# 1. A vigild that dies at every start: started again, then given up on.
reset segv
supervise
if [ "$(starts)" = 5 ] && grep -q '^signal=11$' "$SB/crash/vigil.loop" 2>/dev/null; then
    ok "a vigild that crashes at every start is run five times, then left stopped with /etc/crash/vigil.loop"
else
    bad "crash loop: $(starts) starts, loop note: $(ls "$SB/crash")"
fi
grep -q 'not starting it again' "$SB/log" 2>/dev/null && ok "and the log says so" || bad "log: $(cat "$SB/log" 2>/dev/null)"

# 2. Stopped on purpose, or ending by itself: never started again; a failure
#    of its own is logged with its status.
for m in term exit noso; do
    reset $m
    supervise
    [ "$(starts)" = 1 ] && [ ! -f "$SB/crash/vigil.loop" ] && ok "a vigild that ends with '$m' is not started again" \
        || bad "$m: $(starts) starts"
done
grep -q 'exited with status 127' "$SB/log" 2>/dev/null && ok "an exit of its own is logged with its status" \
    || bad "exit status not logged: $(cat "$SB/log" 2>/dev/null)"

# 3. An upgrade has the board.
reset segv
touch "$SB/sysupgrade.lock"
supervise
[ "$(starts)" = 0 ] && ok "during an upgrade vigild is not started" || bad "upgrade: $(starts) starts"

# 4. Arguments from the configuration.
reset exit
supervise
[ "$(args)" = "-s -o none" ] && ok "no configuration: no cameras, no recording, syslog" || bad "empty config: $(args)"

reset exit
printf 'rtsp://u:p@10.0.0.1/stream=0\nnetip://admin:@10.0.0.2:34567/?ch=1&stream=extra\n' > "$SB/cfg/.cameras"
touch "$SB/x1" "$SB/x2"         # would match a '?' glob if one were expanded
supervise
[ "$(args)" = "-s -c rtsp://u:p@10.0.0.1/stream=0 -c netip://admin:@10.0.0.2:34567/?ch=1&stream=extra -o none" ] \
    && ok "each camera URL is one -c, '?' and '&' untouched" || bad "cameras: $(args)"

reset exit
mkdir -p "$SB/disk/My Recordings"
echo "$SB/disk/My Recordings" > "$SB/cfg/.record.path"
echo 300 > "$SB/cfg/.record.segment"
echo 5 > "$SB/cfg/.record.keep"
supervise
if [ "$(sed -n 3p "$SB/args")" = "$SB/disk/My Recordings" ] && [ "$(sed -n 2p "$SB/args")" = -o ] \
    && [ "$(sed -n '4,7p' "$SB/args" | tr '\n' ' ')" = "-S 300 -K 5 " ]; then
    ok "a recording directory on a disk is passed as one argument, spaces and all, with -S and -K"
else
    bad "disk path: $(args)"
fi

reset exit
echo "$SB/ram" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "-s -o none" ] && grep -q "not recording: $SB/ram" "$SB/log" \
    && ok "a directory on tmpfs is refused, and the log says so" || bad "tmpfs path: $(args)"

reset exit
ln -s "$SB/ram" "$SB/disk/rec"
echo "$SB/disk/rec" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "-s -o none" ] && ok "a link on the disk that leads to tmpfs is refused" || bad "linked path: $(args)"

reset exit
echo "$SB/proc/self" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "-s -o none" ] && ok "a directory on proc (not a disk, card or share) is refused" || bad "proc path: $(args)"

reset exit
touch "$SB/cfg/.broken"
supervise
if [ "$(args)" = "-s -o none" ] && grep -q "cannot read .*Parse error" "$SB/log" 2>/dev/null; then
    ok "a configuration yaml-cli cannot parse is said in the log, not taken for an empty one"
else
    bad "broken config: args '$(args)', log '$(cat "$SB/log" 2>/dev/null)'"
fi
rm -f "$SB/cfg/.broken"

reset exit
mkdir -p "$SB/flash"
echo "$SB/flash" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "-s -o none" ] && ok "a directory on the flash root is refused" || bad "root path: $(args)"

reset exit
mkdir -p "$SB/disk/real"
ln -s "$SB/disk/real" "$SB/disk/alias"
echo "$SB/disk/alias" > "$SB/cfg/.record.path"
supervise
[ "$(sed -n 3p "$SB/args")" = "$SB/disk/real" ] && ok "a link within the disk is followed, and vigild gets where it leads" \
    || bad "alias path: $(args)"

reset exit
mkdir -p "$SB/Disk One/rec"
echo "$SB/Disk One/rec" > "$SB/cfg/.record.path"
supervise
[ "$(sed -n 3p "$SB/args")" = "$SB/Disk One/rec" ] && ok "a disk mounted at a path with a space (escaped in /proc/mounts) is recorded to" \
    || bad "escaped mount point: $(args)"

# 5. A card mounted after vigild started: found, and vigild started again with it.
reset run
echo "$SB/card/rec" > "$SB/cfg/.record.path"
sh "$SB/S95vigil" start > /dev/null
for _ in $(seq 50); do [ "$(starts)" -ge 1 ] && break; sleep 0.1; done
first=$(args)
mkdir -p "$SB/card/rec"
echo "/dev/mmcblk0p1 $SB/card vfat rw 0 0" >> "$SB/mounts"
for _ in $(seq 60); do [ "$(starts)" -ge 2 ] && break; sleep 0.1; done
sleep 0.3
if [ "$first" = "-s -o none" ] && [ "$(starts)" = 2 ] && [ "$(sed -n 3p "$SB/args")" = "$SB/card/rec" ]; then
    ok "a card mounted later is found, and vigild is started again to record to it"
else
    bad "late card: first '$first', starts $(starts), now '$(args)'"
fi
if pgrep -f "$SB/S95vigil supervise" > /dev/null && [ ! -f "$SB/crash/vigil.loop" ]; then
    ok "...by the same supervisor, with no crash counted"
else
    bad "late card: supervisor gone or a crash counted"
fi
sh "$SB/S95vigil" stop > /dev/null
sleep 0.5
cp "$SB/mounts.boot" "$SB/mounts"

# 6. A lock left with no pid -- its command killed between mkdir and writing
#    its pid -- is taken over after a short grace, not waited on forever.
reset run
mkdir -p "$SB/run/vigil-supervise.pid.lock"
if timeout 15 sh "$SB/S95vigil" start > /dev/null; then
    ok "a lock its owner never wrote a pid into is taken over"
else
    bad "start waited on a pid-less lock"
fi
sh "$SB/S95vigil" stop > /dev/null
sleep 0.5

# 7. start and stop.
reset run
echo "utc=2026-01-01 00:00:00" > "$SB/crash/vigil.loop"
sh "$SB/S95vigil" start > /dev/null
for _ in $(seq 50); do [ "$(starts)" -ge 1 ] && break; sleep 0.1; done
sleep 0.3
[ ! -f "$SB/crash/vigil.loop" ] && ok "a start clears the note that the last run was given up on" || bad "loop note survived a start"
pgrep -f "$SB/S95vigil supervise" > /dev/null && ok "start leaves a supervisor running vigild" || bad "no supervisor after start"
sh "$SB/S95vigil" stop > /dev/null
sleep 0.5
if ! pgrep -f "$SB/S95vigil supervise" > /dev/null && ! pgrep -f "^sleep 1000" > /dev/null && [ "$(starts)" = 1 ]; then
    ok "stop ends the supervisor and vigild, and nothing starts it again"
else
    bad "after stop: supervisor $(pgrep -f "$SB/S95vigil supervise"), starts $(starts)"
fi

echo
if [ "$fail" -eq 0 ]; then
    echo "All S95vigil checks passed."
else
    echo "$fail S95vigil check(s) failed."
    exit 1
fi
