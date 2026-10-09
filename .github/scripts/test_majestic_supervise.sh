#!/bin/bash
# Regression tests for S95majestic's supervisor: majestic started again after
# a crash, and the camera never held in a restart loop.
#
# Everything it gets wrong is silent, and on a camera nobody is watching: a
# majestic left dead after a crash (no video until a power cycle), one that
# dies at every start restarted forever, one stopped on purpose -- by
# `S95majestic stop`, by sysupgrade -- started again behind the stopper's
# back, or a stop that returns while a restart is still on its way. None of
# it happens on demand without a majestic that crashes.
#
# Run the real script in a sandbox: its paths are rewritten under $SB, its
# waits shortened, and majestic and start-stop-daemon are stubs. The stub
# start-stop-daemon does what busybox's does with the flags the script uses:
# -S runs the daemon in place, -K signals it, -t -K says whether it runs.
#
# Lightweight: pure shell, no root, runs in a few seconds.

set -u

SRC=${SRC:-general/package/majestic/files/S95majestic}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

[ -f "$SRC" ] || { echo "FAIL cannot find $SRC — run me from the repo root"; exit 1; }

SB=$(mktemp -d)
trap 'pkill -f "$SB/" 2>/dev/null; rm -rf "$SB"' EXIT
mkdir -p "$SB/bin" "$SB/run" "$SB/crash"

sed -e "s|DAEMON_PATH=\"/usr/bin/\$DAEMON\"|DAEMON_PATH=\"$SB/bin/majestic\"|" \
    -e "s|/var/run/|$SB/run/|g" \
    -e "s|CRASH=/etc/crash|CRASH=$SB/crash|" \
    -e "s|UPGRADE_LOCK=/tmp/sysupgrade.lock|UPGRADE_LOCK=$SB/sysupgrade.lock|" \
    -e "s|BACKOFF=\"5 30 120\"|BACKOFF=\"\${TEST_BACKOFF:-0 0 0}\"|" \
    "$SRC" > "$SB/S95majestic"
for p in 'DAEMON_PATH="/usr/bin/$DAEMON"' /var/run/ CRASH=/etc/crash UPGRADE_LOCK=/tmp/sysupgrade.lock 'BACKOFF="5 30 120"'; do
    grep -qF "$p" "$SRC" || bad "$p is gone from S95majestic; this test is testing nothing there"
done
chmod +x "$SB/S95majestic"

# majestic: counts its starts, then does what $SB/mode says -- dies of a
# signal, exits, or runs until it is stopped.
cat > "$SB/bin/majestic" <<EOF
#!/bin/sh
echo start >> "$SB/starts"
case "\$(cat "$SB/mode")" in
    segv) kill -SEGV \$\$ ;;
    abrt) kill -ABRT \$\$ ;;
    term) kill -TERM \$\$ ;;
    exit) exit 0 ;;
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
printf '#!/bin/sh\necho "$*" >> "%s/log"\n' "$SB" > "$SB/bin/logger"
chmod +x "$SB/bin"/*
export PATH="$SB/bin:$PATH"

starts() { [ -f "$SB/starts" ] && wc -l < "$SB/starts" | tr -d ' ' || echo 0; }
reset() { rm -f "$SB/starts" "$SB/log" "$SB/crash"/* "$SB/sysupgrade.lock" "$SB/run"/*; echo "$1" > "$SB/mode"; }
supervise() { timeout 20 sh "$SB/S95majestic" supervise; }

# 1. A majestic that dies at every start: started again, then given up on.
reset segv
supervise
if [ "$(starts)" = 5 ] && [ -f "$SB/crash/majestic.loop" ]; then
    ok "a majestic that crashes at every start is run five times, then left stopped"
else
    bad "crash loop: $(starts) starts, loop note: $(ls "$SB/crash")"
fi
grep -q '^signal=11$' "$SB/crash/majestic.loop" 2>/dev/null && grep -q '^crashes=5$' "$SB/crash/majestic.loop" \
    && grep -q '^utc=' "$SB/crash/majestic.loop" \
    && ok "the loop note says how often, by what signal and when" || bad "loop note: $(cat "$SB/crash/majestic.loop" 2>/dev/null)"
grep -q 'not starting it again' "$SB/log" 2>/dev/null && ok "and the log says so" || bad "log: $(cat "$SB/log" 2>/dev/null)"
[ ! -f "$SB/run/majestic-supervise.pid" ] && ok "the supervisor leaves no pid behind" || bad "pid file left"

# 2. An abort is a crash too.
reset abrt
supervise
[ "$(starts)" = 5 ] && grep -q '^signal=6$' "$SB/crash/majestic.loop" && ok "SIGABRT is restarted like SIGSEGV" \
    || bad "abort: $(starts) starts"

# 3. Stopped on purpose, or ending by itself: never started again.
for m in term exit; do
    reset $m
    supervise
    [ "$(starts)" = 1 ] && [ ! -f "$SB/crash/majestic.loop" ] && ok "a majestic that ends with '$m' is not started again" \
        || bad "$m: $(starts) starts"
done

# 4. An upgrade has the camera: a crash then is left to it.
reset segv
touch "$SB/sysupgrade.lock"
supervise
[ "$(starts)" = 1 ] && [ ! -f "$SB/crash/majestic.loop" ] && ok "during an upgrade a crash is not restarted" \
    || bad "upgrade: $(starts) starts"

# 5. start and stop: the supervisor goes with majestic, and the pid with it.
reset run
echo "utc=2026-01-01 00:00:00" > "$SB/crash/majestic.loop"
sh "$SB/S95majestic" start > /dev/null
for _ in $(seq 50); do [ "$(starts)" -ge 1 ] && break; sleep 0.1; done
sleep 0.3
[ ! -f "$SB/crash/majestic.loop" ] && ok "a start clears the note that the last run was given up on" || bad "loop note survived a start"
pgrep -f "$SB/S95majestic supervise" > /dev/null && ok "start leaves a supervisor running majestic" || bad "no supervisor after start"
out=$(sh "$SB/S95majestic" start)
[ "$out" = "Starting majestic: OK (already running)" ] && ok "a second start starts nothing" || bad "second start: $out"
sh "$SB/S95majestic" stop > /dev/null
sleep 0.5
if ! pgrep -f "$SB/S95majestic supervise" > /dev/null && ! pgrep -f "^sleep 1000" > /dev/null && [ "$(starts)" = 1 ]; then
    ok "stop ends the supervisor and majestic, and nothing starts it again"
else
    bad "after stop: supervisor $(pgrep -f "$SB/S95majestic supervise"), starts $(starts)"
fi
[ ! -f "$SB/run/majestic-supervise.pid" ] && ok "stop removes the supervisor's pid" || bad "pid file left after stop"

# 6. A stop while it waits to start majestic again ends the wait.
reset segv
TEST_BACKOFF="30" sh "$SB/S95majestic" start > /dev/null
for _ in $(seq 50); do [ "$(starts)" -ge 1 ] && break; sleep 0.1; done
sleep 0.3
t0=$(date +%s)
sh "$SB/S95majestic" stop > /dev/null
sleep 0.5
if ! pgrep -f "$SB/S95majestic supervise" > /dev/null && [ $(( $(date +%s) - t0 )) -lt 5 ] && [ "$(starts)" = 1 ]; then
    ok "a stop during the wait after a crash ends it at once"
else
    bad "stop during the wait: supervisor $(pgrep -f "$SB/S95majestic supervise"), starts $(starts)"
fi

# 7. A pid file left by something else is not the supervisor.
reset run
sleep 999 &
other=$!
echo "$other" > "$SB/run/majestic-supervise.pid"
sh "$SB/S95majestic" stop > /dev/null
kill -0 "$other" 2>/dev/null && ok "stop does not kill whatever reused the supervisor's pid" || bad "stop killed pid $other"
kill "$other" 2>/dev/null

[ "$fail" -eq 0 ] && echo "all passed" || echo "$fail failed"
exit "$fail"
