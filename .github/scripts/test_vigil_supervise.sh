#!/bin/bash
# Regression tests for S95vigil: the supervisor it shares with S95majestic
# (restarted after a crash, never in a loop, never during an upgrade, never
# after a stop), and what is its own -- vigild given /etc/vigil.yaml and none
# of its settings, and the one-time conversion of an earlier /etc/vigil.json,
# which vigild itself does (vigild -f vigil.json -w vigil.yaml).
#
# The same sandbox as test_majestic_supervise.sh: the real script with its
# paths rewritten under $SB and its waits shortened; vigild, start-stop-daemon
# and logger are stubs.
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
mkdir -p "$SB/bin" "$SB/run" "$SB/crash" "$SB/etc" "$SB/rom$SB/etc"

sed -e "s|DAEMON_PATH=\"/usr/bin/\$DAEMON\"|DAEMON_PATH=\"$SB/bin/vigild\"|" \
    -e "s|/var/run/|$SB/run/|g" \
    -e "s|CRASH=/etc/crash|CRASH=$SB/crash|" \
    -e "s|UPGRADE_LOCK=/tmp/sysupgrade.lock|UPGRADE_LOCK=$SB/sysupgrade.lock|" \
    -e "s|BACKOFF=\"5 30 120\"|BACKOFF=\"\${TEST_BACKOFF:-0 0 0}\"|" \
    -e "s|CONFIG=/etc/vigil.yaml|CONFIG=$SB/etc/vigil.yaml|" \
    -e "s|LEGACY=/etc/vigil.json|LEGACY=$SB/etc/vigil.json|" \
    -e "s|ROM=/rom|ROM=$SB/rom|" \
    "$SRC" > "$SB/S95vigil"
for p in 'DAEMON_PATH="/usr/bin/$DAEMON"' /var/run/ CRASH=/etc/crash UPGRADE_LOCK=/tmp/sysupgrade.lock 'BACKOFF="5 30 120"' \
    CONFIG=/etc/vigil.yaml LEGACY=/etc/vigil.json ROM=/rom; do
    grep -qF "$p" "$SRC" || bad "$p is gone from S95vigil; this test is testing nothing there"
done
chmod +x "$SB/S95vigil"

# vigild: with -w, converts: writes OUT (0600) from the -f file, or, when
# $SB/wfail exists, refuses as the real one does for a file it cannot load.
# Otherwise counts its starts, keeps its arguments one per line, then does
# what $SB/mode says.
cat > "$SB/bin/vigild" <<EOF
#!/bin/sh
case " \$* " in
*" -w "*)
    printf '%s\n' "\$@" > "$SB/wargs"
    while [ \$# -gt 0 ]; do
        case "\$1" in -f) in=\$2; shift ;; -w) out=\$2; shift ;; esac
        shift
    done
    if [ -f "$SB/wfail" ]; then
        echo "vigil: \$in: cameras[0].url: not a camera URL (schemes: rtsp, netip, capture)" >&2
        echo "vigil: \$in is invalid; using the defaults (the file is kept)" >&2
        echo "vigil: \$in is invalid; not writing \$out" >&2
        exit 1
    fi
    (umask 077; { echo "# YAML of"; cat "\$in"; } > "\$out")
    echo "vigil: \$in: loaded" >&2
    echo "vigil: \$out written" >&2
    exit 0 ;;
esac
echo start >> "$SB/starts"
: > "$SB/args"
for a in "\$@"; do printf '%s\n' "\$a" >> "$SB/args"; done
case "\$(cat "$SB/mode")" in
    segv) kill -SEGV \$\$ ;;
    term) kill -TERM \$\$ ;;
    exit) exit 0 ;;
    noso) exit 127 ;;
    run) exec sleep 1000 ;;
    deaf) trap '' TERM; exec sleep 1000 ;;
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
    start) [ -f "${SB_DIR}/slowstart" ] && sleep 1; exec "$exe" "$@" ;;
    stop)
        pids=$(pgrep -f "^/bin/sh $exe|^sleep 1000" 2>/dev/null)
        [ -n "$pids" ] || exit 1
        [ -n "$test" ] && exit 0
        kill -s "$sig" $pids 2>/dev/null
        exit 0 ;;
esac
EOF
printf '#!/bin/sh\necho "$*" >> "%s/log"\n' "$SB" > "$SB/bin/logger"
chmod +x "$SB/bin"/*
export PATH="$SB/bin:$PATH" SB_DIR="$SB"

starts() { [ -f "$SB/starts" ] && wc -l < "$SB/starts" | tr -d ' ' || echo 0; }
reset() {
    rm -f "$SB/starts" "$SB/args" "$SB/wargs" "$SB/wfail" "$SB/log" "$SB/crash"/* "$SB/sysupgrade.lock" \
        "$SB/run"/* "$SB/etc"/* 2>/dev/null
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

# 4. vigild reads its configuration itself, and decides where it may record
#    (only onto a mounted disk, OpenIPC/vigil#14): it is given the file and
#    none of its settings, whatever the file says.
CONF="$SB/etc/vigil.yaml"
BASE="-s -f $CONF"
reset exit
supervise
[ "$(args)" = "$BASE" ] && ok "vigild is given its configuration file and syslog, nothing else" || bad "empty config: $(args)"

reset exit
printf 'cameras:\n  - url: "rtsp://u:p@10.0.0.1/stream=0"\nrecord:\n  path: /mnt/sda1/rec\n  segment: 300\n' > "$CONF"
supervise
[ "$(args)" = "$BASE" ] && ok "cameras and recording stay in the file, not on the command line" || bad "settings: $(args)"

# 5. An earlier /etc/vigil.json is converted once into /etc/vigil.yaml, by
#    vigild itself.
JSON="$SB/etc/vigil.json"
shipped='# the image'"'"'s vigil.yaml'
mine='{ "version": 1, "cameras": [ { "url": "rtsp://u:p@10.0.0.1/stream=0" } ] }'
migrate_start() {
    sh "$SB/S95vigil" start > /dev/null
    for _ in $(seq 50); do [ "$(starts)" -ge 1 ] && break; sleep 0.1; done
    sh "$SB/S95vigil" stop > /dev/null
    sleep 0.3
}
mode() { stat -c %a "$1" 2>/dev/null; }

reset run
echo "$shipped" > "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo "$mine" > "$JSON"; chmod 644 "$JSON"
migrate_start
[ "$(tr '\n' ' ' < "$SB/wargs" 2>/dev/null)" = "-f $JSON -w $CONF " ] \
    && ok "vigild converts vigil.json itself: -f vigil.json -w vigil.yaml" || bad "conversion: $(cat "$SB/wargs" 2>&1)"
[ "$(head -1 "$CONF")" = "# YAML of" ] && grep -qF "$mine" "$CONF" && ok "...over the vigil.yaml the image ships" \
    || bad "converted: $(cat "$CONF")"
[ ! -e "$JSON" ] && [ -f "$JSON.migrated" ] && [ "$(mode "$JSON.migrated")" = 600 ] \
    && ok "...and the old file is kept as vigil.json.migrated, root only" || bad "old file: $(ls -l "$SB/etc")"
grep -q "converted $JSON into $CONF" "$SB/log" 2>/dev/null && ok "...and the log says so" \
    || bad "conversion log: $(cat "$SB/log" 2>/dev/null)"
[ "$(args)" = "$BASE" ] && ok "...then vigild runs on vigil.yaml" || bad "after conversion: $(args)"
cp "$CONF" "$SB/converted"
rm -f "$SB/wargs"
: > "$SB/log"
migrate_start
cmp -s "$CONF" "$SB/converted" && [ ! -e "$SB/wargs" ] && [ ! -s "$SB/log" ] && ok "a second start converts nothing" \
    || bad "second start: $(cat "$SB/log")"

reset run
rm -f "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo "$mine" > "$JSON"
migrate_start
grep -qF "$mine" "$CONF" 2>/dev/null && [ -f "$JSON.migrated" ] && ok "with no vigil.yaml at all, vigil.json is converted" \
    || bad "no yaml: $(cat "$CONF" 2>&1)"

reset run
printf 'cameras:\n  - url: "rtsp://10.0.0.9/x"\n' > "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo "$mine" > "$JSON"
migrate_start
grep -q 10.0.0.9 "$CONF" && [ -f "$JSON" ] && [ ! -e "$SB/wargs" ] && grep -q "is not converted: .* has been changed since" "$SB/log" \
    && ok "a vigil.yaml changed since (the API, by hand) is not converted over" || bad "changed yaml: $(cat "$CONF"); $(cat "$SB/log")"

reset run
echo "$shipped" > "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo "$mine" > "$JSON"
touch "$SB/wfail"
migrate_start
if [ "$(cat "$CONF")" = "$shipped" ] && [ -f "$JSON" ] && [ ! -e "$JSON.migrated" ] &&
   grep -q "is not converted: $JSON: cameras\[0\].url: not a camera URL" "$SB/log" &&
   ! grep -q -e ': loaded$' -e 'using the defaults' "$SB/log" && [ ! -e "$SB/run/vigil-supervise.pid.err" ]; then
    ok "a vigil.json vigild cannot load is not converted, both files stay, and the log says why"
else
    bad "refused conversion: $(cat "$SB/log"); $(ls "$SB/etc" "$SB/run")"
fi
! grep -q 'u:p@' "$SB/log" 2>/dev/null && ok "...without a password in it" || bad "a password reached the log"
[ "$(args)" = "$BASE" ] && ok "...and vigild runs on vigil.yaml" || bad "after a refused conversion: $(args)"

# 6. A lock left with no pid -- its command killed between mkdir and writing
#    its pid -- is taken over after a short grace, not waited on forever.
reset run
mkdir -p "$SB/run/vigil-supervise.pid.lock"
if timeout 15 sh "$SB/S95vigil" start > /dev/null; then
    ok "a lock its owner never wrote a pid into is taken over"
else
    bad "start waited on a pid-less lock"
    rm -rf "$SB/run/vigil-supervise.pid.lock"      # or the stop below waits on it too
fi
timeout 30 sh "$SB/S95vigil" stop > /dev/null
sleep 0.5

# 7. A vigild that ignores SIGTERM is killed, so stop succeeds and leaves
#    nothing running unsupervised.
reset deaf
sh "$SB/S95vigil" start > /dev/null
for _ in $(seq 50); do [ "$(starts)" -ge 1 ] && break; sleep 0.1; done
sleep 0.3
out=$(timeout 30 sh "$SB/S95vigil" stop)
sleep 0.3
if [ "$out" = "Stopping vigild: OK" ] && ! pgrep -f "^sleep 1000" > /dev/null; then
    ok "a vigild that ignores SIGTERM is killed, and stop succeeds"
else
    bad "deaf vigild: stop said '$out', still running: $(pgrep -f '^sleep 1000')"
fi
pkill -f "^sleep 1000" 2>/dev/null

# 8. A stop that lands while the supervisor is launching vigild, then a start
#     at once: one supervisor, one vigild, and the pid file names the live
#     supervisor -- the old one neither leaves vigild unwatched nor removes
#     the new one's pid file (#2630).
reset run
touch "$SB/slowstart"
sh "$SB/S95vigil" start > /dev/null
sleep 0.3                       # inside the slowed launch
sh "$SB/S95vigil" stop > /dev/null
sh "$SB/S95vigil" start > /dev/null
rm -f "$SB/slowstart"
sleep 2
sups=$(pgrep -f "$SB/S95vigil supervise" | wc -l)
daemons=$(pgrep -f "^sleep 1000" | wc -l)
named=$(cat "$SB/run/vigil-supervise.pid" 2>/dev/null)
if [ "$sups" = 1 ] && [ "$daemons" = 1 ] && [ -n "$named" ] && kill -0 "$named" 2>/dev/null &&
    pgrep -f "$SB/S95vigil supervise" | grep -qx "$named"; then
    ok "a stop during a launch, then a start: one supervisor, one vigild, and its pid file"
else
    bad "stop during launch then start: supervisors $sups, daemons $daemons, pid file '$named'"
fi
sh "$SB/S95vigil" stop > /dev/null
sleep 0.5

# 9. start and stop.
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
