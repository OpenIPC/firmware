#!/bin/bash
# Regression tests for failsafe-log, which keeps the log of the boot that fell
# into failsafe for the crash report.
#
# A boot loop is reported from the camera's next normal boot, and what it can
# say is only what this kept. Boot loops arrived with no log at all while the
# pmsg ring was the only record: the boots that loop mostly end before
# userspace logging starts, and only the kernel console record holds those.
#
# Run the real script against a sandboxed pstore and overlay: the paths it
# hardcodes are rewritten under $SB. Pure shell, no root, runs in a second.

set -u

SRC=${SRC:-general/package/openipc-failsafe/files/failsafe-log}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

[ -f "$SRC" ] || { echo "FAIL cannot find $SRC — run me from the repo root"; exit 1; }

SB=$(mktemp -d)
trap 'rm -rf "$SB"' EXIT
mkdir -p "$SB/pstore"

sed -e "s|/sys/fs/pstore|$SB/pstore|g" -e "s|/etc/crash|$SB/crash|g" "$SRC" > "$SB/failsafe-log"
for p in /sys/fs/pstore /etc/crash; do
    grep -q "$p" "$SRC" || bad "$p is gone from failsafe-log; this test is testing nothing there"
done

LOG=$SB/crash/failsafe.log
run() { sh "$SB/failsafe-log"; }
reset() { rm -rf "$SB/pstore"/* "$SB/crash"; }

# --- nothing kept: nothing written ------------------------------------------
reset
run
[ ! -e "$LOG" ] && ok "no record, no log" || bad "a log was written from nothing"
[ ! -e "$SB/crash" ] && ok "and no directory made for it" || bad "/etc/crash made for nothing"

# --- a boot that ended in the kernel: the console record alone ---------------
reset
printf '[    0.000000] Booting Linux on physical CPU 0x0\n[    3.141592] the boot stopped here\n' \
    > "$SB/pstore/console-ramoops-0"
run
grep -q 'the boot stopped here' "$LOG" 2>/dev/null \
    && ok "a boot that never reached userspace keeps its kernel log" \
    || bad "the console record was not kept"

# --- a boot that got to userspace: the system log alone ----------------------
reset
printf 'Oct 15 09:00:01 cam user.err majestic[812]: the pipeline did not start\n' \
    > "$SB/pstore/pmsg-ramoops-0"
run
grep -q 'the pipeline did not start' "$LOG" 2>/dev/null \
    && ok "the system log is kept when it is all there is" \
    || bad "the pmsg record was not kept"

# --- both: each under its own header, console first ----------------------------
reset
printf '[    1.000000] kernel first\n' > "$SB/pstore/console-ramoops-0"
printf 'Oct 15 09:00:01 cam user.info syslog: userspace after\n' > "$SB/pstore/pmsg-ramoops-0"
run
want=$(printf '==> console-ramoops-0 <==\n[    1.000000] kernel first\n==> pmsg-ramoops-0 <==\nOct 15 09:00:01 cam user.info syslog: userspace after')
[ "$(cat "$LOG" 2>/dev/null)" = "$want" ] \
    && ok "two records, two headed sections, the system log last" \
    || bad "not the two headed sections: $(cat "$LOG" 2>/dev/null)"

# --- a record that cannot be read is left out, not kept in part --------------
if [ "$(id -u)" -ne 0 ]; then
    reset
    printf '[    1.000000] kernel\n' > "$SB/pstore/console-ramoops-0"
    printf 'Oct 15 09:00:01 cam user.info syslog: readable\n' > "$SB/pstore/pmsg-ramoops-0"
    chmod 000 "$SB/pstore/console-ramoops-0"
    run
    chmod 644 "$SB/pstore/console-ramoops-0"
    ! grep -q 'console-ramoops-0' "$LOG" 2>/dev/null && grep -q 'readable' "$LOG" 2>/dev/null \
        && ok "an unreadable record is left out and the other kept" \
        || bad "an unreadable record changed the log: $(cat "$LOG" 2>/dev/null)"
    [ ! -e "$SB/crash/failsafe.log.part" ] && ok "and no part file is left" || bad "failsafe.log.part left behind"
else
    echo "skip the unreadable-record case: root reads a mode-000 file"
fi

# --- an earlier fall's log never sits beside this one's note -----------------
reset
mkdir -p "$SB/crash"
echo 'an older boot' > "$LOG"
run
[ ! -e "$LOG" ] && ok "an earlier fall's log is dropped when this one left none" \
    || bad "an earlier fall's log survived: $(cat "$LOG")"
[ ! -e "$LOG.new" ] && ok "and nothing half-written is left" || bad "failsafe.log.new left behind"

echo
[ "$fail" -eq 0 ] && echo "all passed" || echo "$fail failed"
exit "$fail"
