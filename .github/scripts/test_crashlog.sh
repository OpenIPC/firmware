#!/bin/bash
# Regression tests for S98crashlog, the capture half of the crash report.
#
# Everything it gets wrong is silent and happens on the boot after a crash, on
# a camera nobody is watching: a crash that is never harvested, a harvest that
# erases the pstore records it failed to keep, an earlier crash overwritten by
# a later one before anyone looked, or a meta.json that carries the owner's
# passwords off the camera when the bundle is sent. None of it can be made to
# happen on demand without crashing a camera.
#
# Run the real script against a sandboxed device: the paths it hardcodes are
# rewritten under $SB, and the camera's tools are stubs on PATH.
#
# Lightweight: pure shell, no root, runs in a second.

set -u

SRC=${SRC:-general/package/openipc-failsafe/files/S98crashlog}
fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

[ -f "$SRC" ] || { echo "FAIL cannot find $SRC — run me from the repo root"; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "FAIL python3 is needed to check meta.json is JSON"; exit 1; }

SB=$(mktemp -d)
trap 'rm -rf "$SB"' EXIT
mkdir -p "$SB/pstore" "$SB/etc" "$SB/bin"

sed -e "s|/sys/fs/pstore|$SB/pstore|g" \
    -e "s|/etc/crash|$SB/etc/crash|g" \
    -e "s|/etc/majestic.yaml|$SB/etc/majestic.yaml|g" \
    -e "s|/etc/os-release|$SB/etc/os-release|g" \
    -e "s|/proc/cmdline|$SB/cmdline|g" \
    "$SRC" > "$SB/S98crashlog"
for p in /sys/fs/pstore /etc/crash /etc/majestic.yaml /etc/os-release /proc/cmdline; do
    grep -q "$p" "$SRC" || bad "$p is gone from S98crashlog; this test is testing nothing there"
done

cat > "$SB/bin/ipcinfo" <<'EOF'
#!/bin/sh
case "$1" in -v) echo goke ;; --chip-name) echo gk7205v300 ;; esac
EOF
cat > "$SB/bin/fw_printenv" <<'EOF'
#!/bin/sh
echo imx335
EOF
cat > "$SB/bin/majestic" <<'EOF'
#!/bin/sh
echo 'Lite, master+0000000, 2026-01-01 00:00'
EOF
printf '#!/bin/sh\nexit 0\n' > "$SB/bin/logger"
chmod +x "$SB/bin"/*

cat > "$SB/etc/os-release" <<'EOF'
OPENIPC_VERSION=2.6.10.05
GITHUB_VERSION="master+0000000, 2026-01-01"
BUILD_OPTION=lite
BUILD_ID=nightly-20260101-0000000
BUILD_PLATFORM=gk7205v300_lite
EOF
echo 'mem=128M console=ttyAMA0,115200' > "$SB/cmdline"
cat > "$SB/etc/majestic.yaml" <<'EOF'
# Managed by majestic.
system:
  webAdmin: example
video0:
  codec: h265
  fps: 25
osd:
  enabled: true
  overlays:
    1:
      template: Front door "quoted"
      posX: -16
audio:
  volume: 30
outgoing:
  server: rtmp://example.invalid/live/streamkey
netip:
  password: hunter2
rtsp:
  user: admin
  port: 554
EOF

run() { PATH="$SB/bin:$PATH" sh "$SB/S98crashlog" start; }
crash() { printf '%s\n' "$1" > "$SB/pstore/dmesg-ramoops-0"; }
listing() { gzip -dc "$1" | tar -tf - | sed 's|^\./||' | grep -v '^$' | sort | tr '\n' ' '; }
B=$SB/etc/crash

# --- a clean boot -----------------------------------------------------------
run
[ ! -e "$B" ] && ok "a boot with no records writes nothing" || bad "a clean boot left $B behind"

# --- the first crash --------------------------------------------------------
crash "Oops one"
run
[ -s "$B/crash.tar.gz" ] && ok "the crash is preserved" || bad "no crash.tar.gz"
[ "$(listing "$B/crash.tar.gz")" = "dmesg-ramoops-0 meta.json " ] \
    && ok "the bundle holds the record and meta.json" || bad "bundle holds: $(listing "$B/crash.tar.gz")"
[ ! -e "$SB/pstore/dmesg-ramoops-0" ] && ok "the pstore ring is freed once the bundle is in place" || bad "records left in pstore"
grep -q '^records=1$' "$B/pending" && grep -q '^older=0$' "$B/pending" \
    && ok "pending says one record, nothing older" || bad "pending: $(cat "$B/pending")"

meta=$(gzip -dc "$B/crash.tar.gz" | tar -xOf - ./meta.json 2>/dev/null || gzip -dc "$B/crash.tar.gz" | tar -xOf - meta.json)
if printf '%s' "$meta" | python3 -c 'import json,sys; m=json.load(sys.stdin); sys.exit(0 if m["soc"]=="gk7205v300" and m["sensor"]=="imx335" and m["firmware"]["version"]=="2.6.10.05" and m["cmdline"].startswith("mem=128M") else 1)'; then
    ok "meta.json is JSON, and says what was running"
else
    bad "meta.json: $meta"
fi
cfg=$(printf '%s' "$meta" | python3 -c 'import json,sys; print("\n".join(json.load(sys.stdin)["config"]))')
printf '%s\n' "$cfg" | grep -qx '  codec: h265' && printf '%s\n' "$cfg" | grep -qx '  port: 554' \
    && ok "the pipeline's settings are in it" || bad "config: $cfg"
for leak in hunter2 streamkey example.invalid 'Front door' admin webAdmin; do
    printf '%s' "$meta" | grep -q "$leak" && bad "meta.json carries '$leak'"
done
printf '%s' "$meta" | grep -q -E 'hunter2|streamkey|Front door|admin' || ok "no password, server, own text or account leaves the camera"

# --- crashes before anyone looked ------------------------------------------
sed -i 's/^utc=.*/utc=2026-01-01 00:00:01/' "$B/pending"
crash "Oops two"
run
[ -s "$B/older/20260101000001.tar.gz" ] && ok "the earlier crash is kept, named by when it was captured" \
    || bad "older/: $(ls "$B/older" 2>/dev/null)"
gzip -dc "$B/crash.tar.gz" | tar -xOf - ./dmesg-ramoops-0 2>/dev/null | grep -q 'Oops two' \
    && ok "crash.tar.gz is the latest" || bad "crash.tar.gz is not the second crash"
for i in 2 3; do
    sed -i "s/^utc=.*/utc=2026-01-0$i 00:00:00/" "$B/pending"
    crash "Oops $((i + 1))"
    run
done
[ "$(ls -1 "$B/older" | tr '\n' ' ')" = "20260102000000.tar.gz 20260103000000.tar.gz " ] \
    && ok "at most two older crashes, the oldest dropped first" || bad "older/: $(ls "$B/older" | tr '\n' ' ')"
grep -q '^older=2$' "$B/pending" && ok "pending counts them" || bad "pending: $(cat "$B/pending")"

# --- a capture that fails keeps the evidence --------------------------------
if [ "$(id -u)" != 0 ]; then
    before=$(md5sum < "$B/crash.tar.gz")
    crash "Oops unreadable"
    chmod 000 "$SB/pstore/dmesg-ramoops-0"
    run
    chmod 644 "$SB/pstore/dmesg-ramoops-0"
    [ -e "$SB/pstore/dmesg-ramoops-0" ] && ok "a record it could not read stays in pstore" || bad "an unread record was deleted"
    [ "$(md5sum < "$B/crash.tar.gz")" = "$before" ] && ok "and the last good bundle is untouched" || bad "the bundle changed on a failed capture"
    [ ! -e "$B/stage" ] && ok "no staging left behind" || bad "stage directory left behind"
else
    echo "skip the unreadable-record case: root reads a mode-000 file"
fi

echo
[ "$fail" -eq 0 ] && echo "all passed" || echo "$fail failed"
exit "$fail"
