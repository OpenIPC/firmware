#!/bin/bash
# Regression tests for S95vigil: the supervisor it shares with S95majestic
# (restarted after a crash, never in a loop, never during an upgrade, never
# after a stop), and what is its own -- vigild given /etc/vigil.json and none
# of its settings, the recording it holds off while the directory is not on a
# disk, and the one-time conversion of an old /etc/vigil.yaml.
#
# The same sandbox as test_majestic_supervise.sh: the real script with its
# paths rewritten under $SB and its waits shortened; vigild, start-stop-daemon,
# yaml-cli and logger are stubs, and /proc/mounts is a file the test writes.
# The conversion's JSON is checked with python3.
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
mkdir -p "$SB/bin" "$SB/run" "$SB/crash" "$SB/cfg" "$SB/legacy" "$SB/etc" "$SB/rom$SB/etc" \
    "$SB/disk" "$SB/ram" "$SB/proc/self"

sed -e "s|DAEMON_PATH=\"/usr/bin/\$DAEMON\"|DAEMON_PATH=\"$SB/bin/vigild\"|" \
    -e "s|/var/run/|$SB/run/|g" \
    -e "s|CRASH=/etc/crash|CRASH=$SB/crash|" \
    -e "s|UPGRADE_LOCK=/tmp/sysupgrade.lock|UPGRADE_LOCK=$SB/sysupgrade.lock|" \
    -e "s|BACKOFF=\"5 30 120\"|BACKOFF=\"\${TEST_BACKOFF:-0 0 0}\"|" \
    -e "s|/proc/mounts|$SB/mounts|" \
    -e "s|CARD_POLL=10|CARD_POLL=1|" \
    -e "s|CONFIG=/etc/vigil.json|CONFIG=$SB/etc/vigil.json|" \
    -e "s|LEGACY=/etc/vigil.yaml|LEGACY=$SB/etc/vigil.yaml|" \
    -e "s|ROM=/rom|ROM=$SB/rom|" \
    -e "s|HELD=/etc/vigil.rec-held|HELD=$SB/etc/vigil.rec-held|" \
    "$SRC" > "$SB/S95vigil"
for p in 'DAEMON_PATH="/usr/bin/$DAEMON"' /var/run/ CRASH=/etc/crash UPGRADE_LOCK=/tmp/sysupgrade.lock 'BACKOFF="5 30 120"' /proc/mounts CARD_POLL=10 \
    CONFIG=/etc/vigil.json LEGACY=/etc/vigil.yaml ROM=/rom HELD=/etc/vigil.rec-held; do
    grep -qF "$p" "$SRC" || bad "$p is gone from S95vigil; this test is testing nothing there"
done
chmod +x "$SB/S95vigil"

# A disk, a RAM disk and the flash root, as /proc/mounts would list them.
# A mount point with a space in it is listed with the space escaped (\040).
mkdir -p "$SB/Disk One" "$SB/rocard/rec" "$SB/back\\slash/rec"
cat > "$SB/mounts" <<EOF
/dev/root / squashfs ro 0 0
tmpfs $SB/ram tmpfs rw 0 0
/dev/sda1 $SB/disk ext4 rw 0 0
/dev/sdb1 $SB/Disk\\040One ext4 rw 0 0
proc $SB/proc proc rw 0 0
/dev/sdc1 $SB/rocard vfat ro,relatime 0 0
/dev/sdd1 $SB/back\\134slash ext4 rw 0 0
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
# yaml-cli -i FILE -g .a.b: the value is the file $SB/cfg/.a.b for vigil.json,
# $SB/legacy/.a.b for vigil.yaml; absent = not set (exit 1, nothing said); with
# .broken present there it fails to parse, as the real one does.
cat > "$SB/bin/yaml-cli" <<EOF
#!/bin/sh
case "\$2" in *.yaml) d="$SB/legacy" ;; *) d="$SB/cfg" ;; esac
if [ -f "\$d/.broken" ]; then
    echo "Parse error: while parsing a block collection at line 2, column 3" >&2
    exit 1
fi
[ "\$3" = -g ] && [ -f "\$d/\$4" ] && cat "\$d/\$4"
EOF
printf '#!/bin/sh\necho "$*" >> "%s/log"\n' "$SB" > "$SB/bin/logger"
chmod +x "$SB/bin"/*
export PATH="$SB/bin:$PATH" SB_DIR="$SB"

starts() { [ -f "$SB/starts" ] && wc -l < "$SB/starts" | tr -d ' ' || echo 0; }
reset() {
    rm -f "$SB/starts" "$SB/args" "$SB/log" "$SB/crash"/* "$SB/sysupgrade.lock" "$SB/run"/* "$SB/cfg"/.*[a-z]* \
        "$SB/legacy"/.*[a-z]* "$SB/etc"/* 2>/dev/null
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

# 4. vigild reads its configuration itself: it is given the file and none of
#    its settings, except recording held off while the directory is not on a
#    disk or card.
CONF="$SB/etc/vigil.json"
BASE="-s -f $CONF"
reset exit
supervise
[ "$(args)" = "$BASE" ] && ok "vigild is given its configuration file and syslog, nothing else" || bad "empty config: $(args)"

reset exit
printf 'rtsp://u:p@10.0.0.1/stream=0\n' > "$SB/cfg/.cameras"
echo 300 > "$SB/cfg/.record.segment"
supervise
[ "$(args)" = "$BASE" ] && ok "cameras and segment length stay in the file, not on the command line" || bad "cameras: $(args)"

reset exit
mkdir -p "$SB/disk/My Recordings"
echo "$SB/disk/My Recordings" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE" ] && ok "a recording directory on a disk: recorded, as the file says" || bad "disk path: $(args)"

reset exit
echo "$SB/disk/not/yet" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE" ] && ok "a directory not made yet on a mounted disk: left to vigild to create" || bad "new dir on disk: $(args)"

reset exit
echo "$SB/card/rec" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE -o none" ] && grep -q "not recording for now: $SB/card/rec" "$SB/log" \
    && ok "a card not mounted: recording held off (vigild would create the path on the flash root), and logged" \
    || bad "card not mounted: $(args)"

reset exit
echo "$SB/ram" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE -o none" ] && grep -q "not recording for now: $SB/ram" "$SB/log" \
    && ok "a directory on tmpfs is refused, and the log says so" || bad "tmpfs path: $(args)"

reset exit
ln -s "$SB/ram" "$SB/disk/rec"
echo "$SB/disk/rec/cams" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE -o none" ] && ok "a link on the disk that leads to tmpfs is refused" || bad "linked path: $(args)"

reset exit
echo "$SB/proc/self" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE -o none" ] && ok "a directory on proc (not a disk, card or share) is refused" || bad "proc path: $(args)"

reset exit
echo "$SB/rocard/rec" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE -o none" ] && ok "a card mounted read-only is refused" || bad "read-only card: $(args)"

reset exit
echo "$SB/disk/x/../../../escape" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE -o none" ] && ok "a path that climbs out of the disk with '..' is refused" || bad "dot-dot path: $(args)"

reset exit
echo "$SB/back\\slash/rec/" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE" ] && ok "a disk mounted at a path with a backslash is recorded to (trailing slash and all)" \
    || bad "backslash mount point: $(args)"

reset exit
touch "$SB/cfg/.broken"
supervise
if [ "$(args)" = "$BASE" ] && grep -q "cannot check the recording directory .*Parse error" "$SB/log" 2>/dev/null; then
    ok "a configuration yaml-cli cannot parse is said in the log, and left to vigild"
else
    bad "broken config: args '$(args)', log '$(cat "$SB/log" 2>/dev/null)'"
fi
rm -f "$SB/cfg/.broken"

reset exit
mkdir -p "$SB/flash"
echo "$SB/flash" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE -o none" ] && ok "a directory on the flash root is refused" || bad "root path: $(args)"

reset exit
mkdir -p "$SB/disk/real"
ln -s "$SB/disk/real" "$SB/disk/alias"
echo "$SB/disk/alias" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE" ] && ok "a link within the disk is followed, and recorded to" || bad "alias path: $(args)"

reset exit
mkdir -p "$SB/Disk One/rec"
echo "$SB/Disk One/rec" > "$SB/cfg/.record.path"
supervise
[ "$(args)" = "$BASE" ] && ok "a disk mounted at a path with a space (escaped in /proc/mounts) is recorded to" \
    || bad "escaped mount point: $(args)"

# 5. A card mounted after vigild started: found, and vigild started again,
#    now recording as the file says.
reset run
echo "$SB/card/rec" > "$SB/cfg/.record.path"
sh "$SB/S95vigil" start > /dev/null
for _ in $(seq 50); do [ "$(starts)" -ge 1 ] && break; sleep 0.1; done
first=$(args)
mkdir -p "$SB/card"
echo "/dev/mmcblk0p1 $SB/card vfat rw 0 0" >> "$SB/mounts"
for _ in $(seq 60); do [ "$(starts)" -ge 2 ] && break; sleep 0.1; done
sleep 0.3
if [ "$first" = "$BASE -o none" ] && [ "$(starts)" = 2 ] && [ "$(args)" = "$BASE" ]; then
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

# 5b. Held off, an API change saves record.path as empty meanwhile (what is
#     in force): the path asked for is kept, and recorded into, with -o, once
#     the disk is there. vigild's next save puts it back in the file.
HELDF="$SB/etc/vigil.rec-held"
wait_starts() { for _ in $(seq 60); do [ "$(starts)" -ge "$1" ] && break; sleep 0.1; done; sleep 0.3; }
reset run
echo '{"record":{"path":"held"}}' > "$CONF"
echo "$SB/card/rec" > "$SB/cfg/.record.path"
sh "$SB/S95vigil" start > /dev/null
wait_starts 1
[ "$(cat "$HELDF" 2>/dev/null)" = "$SB/card/rec" ] && ok "the path held off is remembered" || bad "held file: $(cat "$HELDF" 2>&1)"
rm -f "$SB/cfg/.record.path"; echo '{"record":{"path":""}}' > "$CONF"
sleep 2.5
[ "$(starts)" = 1 ] && ok "record.path saved empty while held off: vigild is not started again for it" || bad "empty save: starts $(starts)"
mkdir -p "$SB/card"
echo "/dev/mmcblk0p1 $SB/card vfat rw 0 0" >> "$SB/mounts"
wait_starts 2
[ "$(starts)" = 2 ] && [ "$(args)" = "$BASE -o $SB/card/rec" ] && grep -q "saved empty while recording was held off" "$SB/log" \
    && ok "...and once the disk is there, vigild records into the path asked for (-o), and the log says why" \
    || bad "restore: starts $(starts), args '$(args)'"
echo "$SB/card/rec" > "$SB/cfg/.record.path"; echo '{"record":{"path":"saved back"}}' > "$CONF"
sleep 2.5
[ ! -e "$HELDF" ] && [ "$(starts)" = 2 ] && ok "vigild's next save puts the path back; nothing is held any more, no restart" \
    || bad "after save-back: held '$(cat "$HELDF" 2>&1)', starts $(starts)"

# 5c. Recording, and then the disk goes away: held off at once (vigild would
#     create the path on the flash), not at the next start.
cp "$SB/mounts.boot" "$SB/mounts"
wait_starts 3
[ "$(starts)" = 3 ] && [ "$(args)" = "$BASE -o none" ] && grep -q "is not on a disk or card now" "$SB/log" \
    && ok "a disk gone while recording: vigild started again with recording held off" || bad "disk gone: starts $(starts), '$(args)'"
sh "$SB/S95vigil" stop > /dev/null
sleep 0.5

# 5d. A path the API sets while vigild runs that is not on a disk: held off.
reset run
mkdir -p "$SB/disk/live"
echo "$SB/disk/live" > "$SB/cfg/.record.path"
sh "$SB/S95vigil" start > /dev/null
wait_starts 1
echo "$SB/ram/rec" > "$SB/cfg/.record.path"
wait_starts 2
[ "$(starts)" = 2 ] && [ "$(args)" = "$BASE -o none" ] && ok "a path set while running onto tmpfs: held off within a poll" \
    || bad "live unsafe path: starts $(starts), '$(args)'"
sh "$SB/S95vigil" stop > /dev/null
sleep 0.5

# 5e. Started with -o and a held path, the owner turns recording off through
#     the API: the save (path empty, the file changed) is theirs, and nothing
#     is held any more.
reset run
mkdir -p "$SB/disk/live"
echo "$SB/disk/live" > "$HELDF"
echo '{"record":{"path":""}}' > "$CONF"
sh "$SB/S95vigil" start > /dev/null
wait_starts 1
first=$(args)
echo '{"record":{"path":"", "saved":1}}' > "$CONF"
sleep 2.5
sh "$SB/S95vigil" stop > /dev/null
sleep 0.5
sh "$SB/S95vigil" start > /dev/null
wait_starts 2
if [ "$first" = "$BASE -o $SB/disk/live" ] && [ ! -e "$HELDF" ] && [ "$(args)" = "$BASE" ]; then
    ok "recording turned off through the API after a restore: not restored again"
else
    bad "turned off: first '$first', held '$(cat "$HELDF" 2>&1)', now '$(args)'"
fi
sh "$SB/S95vigil" stop > /dev/null
sleep 0.5

# 5f. An old /etc/vigil.yaml is converted once into /etc/vigil.json.
shipped='{ "version": 1, "cameras": [] }'
migrate_start() {
    sh "$SB/S95vigil" start > /dev/null
    for _ in $(seq 50); do [ "$(starts)" -ge 1 ] && break; sleep 0.1; done
    sh "$SB/S95vigil" stop > /dev/null
    sleep 0.3
}
json() { python3 -c 'import json,sys; print(json.dumps(json.load(open(sys.argv[1])), sort_keys=True))' "$1" 2>&1; }
mode() { stat -c %a "$1" 2>/dev/null; }

reset run
echo "$shipped" > "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo 'cameras: ...' > "$SB/etc/vigil.yaml"
printf 'rtsp://u:p@10.0.0.1/stream=0\nnetip://admin:@10.0.0.2:34567/?ch=1&stream=extra\n' > "$SB/legacy/.cameras"
printf '%s\n' '/mnt/sda1/my "rec"\dir' > "$SB/legacy/.record.path"
echo 300 > "$SB/legacy/.record.segment"
echo 5 > "$SB/legacy/.record.keep"
migrate_start
want='{"cameras": [{"url": "rtsp://u:p@10.0.0.1/stream=0"}, {"url": "netip://admin:@10.0.0.2:34567/?ch=1&stream=extra"}], "record": {"keep": 5, "path": "/mnt/sda1/my \"rec\"\\dir", "segment": 300}, "version": 1}'
if [ "$(json "$CONF")" = "$want" ]; then
    ok "vigil.yaml is converted: cameras, recording path (quotes, backslash), segment and keep"
else
    bad "converted: $(json "$CONF")"
fi
[ "$(mode "$CONF")" = 600 ] && ok "...into a vigil.json only root can read (camera passwords)" || bad "vigil.json mode $(mode "$CONF")"
[ ! -e "$SB/etc/vigil.yaml" ] && [ -f "$SB/etc/vigil.yaml.migrated" ] && [ "$(mode "$SB/etc/vigil.yaml.migrated")" = 600 ] \
    && ok "...and the old file is kept as vigil.yaml.migrated, root only" || bad "old file: $(ls -l "$SB/etc")"
grep -q "converted $SB/etc/vigil.yaml into $CONF: 2 camera(s)" "$SB/log" 2>/dev/null && ok "...and the log says so" \
    || bad "conversion log: $(cat "$SB/log" 2>/dev/null)"
! grep -q 'u:p@' "$SB/log" 2>/dev/null && ok "...without a password in it" || bad "a password reached the log"
cp "$CONF" "$SB/converted"
: > "$SB/log"
migrate_start
cmp -s "$CONF" "$SB/converted" && [ ! -s "$SB/log" ] && ok "a second start converts nothing" || bad "second start: $(cat "$SB/log")"

reset run
echo "$shipped" > "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo 'cameras: ...' > "$SB/etc/vigil.yaml"
printf 'rtsp://u:p@10.0.0.1/a\nfoo://bar\nrtsp://u:p@10.0.0.3/c\n' > "$SB/legacy/.cameras"
echo 'rec' > "$SB/legacy/.record.path"
echo 5 > "$SB/legacy/.record.segment"
echo 1000 > "$SB/legacy/.record.keep"
migrate_start
want='{"cameras": [{"url": "rtsp://u:p@10.0.0.1/a"}, {"url": "rtsp://u:p@10.0.0.3/c"}], "record": {"keep": 64, "path": "", "segment": 10}, "version": 1}'
[ "$(json "$CONF")" = "$want" ] && grep -q "camera 2 is not a camera URL" "$SB/log" && grep -q "not an absolute path" "$SB/log" \
    && ok "what vigild would refuse is left out or brought into range, so it loads the rest" || bad "ranges: $(json "$CONF"); $(cat "$SB/log")"

reset run
echo "$shipped" > "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo 'cameras: []' > "$SB/etc/vigil.yaml"
migrate_start
want='{"cameras": [], "record": {"path": ""}, "version": 1}'
[ "$(json "$CONF")" = "$want" ] && ok "an empty vigil.yaml converts to no cameras, no recording" || bad "empty: $(json "$CONF")"

reset run
echo '{ "version": 1, "cameras": [ { "url": "rtsp://10.0.0.9/x" } ] }' > "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo 'cameras: ...' > "$SB/etc/vigil.yaml"
printf 'rtsp://u:p@10.0.0.1/a\n' > "$SB/legacy/.cameras"
migrate_start
grep -q 10.0.0.9 "$CONF" && [ -f "$SB/etc/vigil.yaml" ] && grep -q "is not converted: .* has been changed since" "$SB/log" \
    && ok "a vigil.json changed since (the API, by hand) is not converted over" || bad "changed json: $(cat "$CONF"); $(cat "$SB/log")"

reset run
echo "$shipped" > "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo 'cameras: [' > "$SB/etc/vigil.yaml"
touch "$SB/legacy/.broken"
migrate_start
[ "$(cat "$CONF")" = "$shipped" ] && [ -f "$SB/etc/vigil.yaml" ] && grep -q "is not converted: Parse error" "$SB/log" \
    && ok "a vigil.yaml yaml-cli cannot parse is not converted, and the log says why" || bad "broken yaml: $(cat "$SB/log")"
rm -f "$SB/legacy/.broken"

reset run
rm -f "$CONF"; echo "$shipped" > "$SB/rom$CONF"
echo 'cameras: ...' > "$SB/etc/vigil.yaml"
printf 'rtsp://u:p@10.0.0.1/a\n' > "$SB/legacy/.cameras"
migrate_start
grep -q 10.0.0.1 "$CONF" 2>/dev/null && ok "with no vigil.json at all, vigil.yaml is converted" || bad "no json: $(cat "$CONF" 2>&1)"

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
