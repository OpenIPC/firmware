#!/bin/sh
# Regression test for the excludes step in general/scripts/rootfs_script.sh.
#
# The per-board scripts/excludes/<model>_<variant>.list files name files by
# hand, so they go stale in one direction with nothing saying so: a package
# renames or drops a sensor blob and the entry that used to prune it silently
# prunes nothing, while the board keeps paying for whatever replaced it.
# OpenIPC/builder's hi3518ev200_lite list names 25 sensor .so files where the
# package now ships 17, which nobody could have known from a build log.
#
# What is checked:
#   Part 1 — behaviour: run the real rootfs_script.sh against a synthetic
#            TARGET_DIR and assert that present entries are removed, absent
#            entries are reported, comments and blank lines are skipped, a
#            file with no trailing newline is still read, and the exit status
#            stays 0 whatever the list says.
#   Part 2 — drift: the script must still route the list through a loop that
#            can tell the two cases apart. A revert to a bare `xargs ... rm`
#            fails here immediately.
#
# Pure shell, no build, runs in under a second.

set -eu

SCRIPT_UNDER_TEST="${SCRIPT_UNDER_TEST:-general/scripts/rootfs_script.sh}"

fail=0
ok()  { echo "ok   $*"; }
bad() { echo "FAIL $*"; fail=$((fail + 1)); }

[ -f "$SCRIPT_UNDER_TEST" ] || { echo "FAIL cannot find $SCRIPT_UNDER_TEST"; exit 1; }

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

# ----- Part 1: behaviour -----
# rootfs_script.sh does a lot besides the excludes step -- os-release stamping,
# the libstdc++ prune, the late-overlay lists. Give it just enough of an
# environment that those are no-ops and only the excludes block does work.
ext="$work/general"
mkdir -p "$ext/scripts/excludes"
target="$work/target"
mkdir -p "$target/usr/lib/sensors" "$target/etc/sensors" "$target/usr/lib"
: > "$target/usr/lib/sensors/libsns_present.so"
: > "$target/etc/sensors/present.ini"
ln -s /nowhere "$target/etc/sensors/broken.ini"     # dangling symlink: -e is false, it must still go
: > "$work/br2config"

# No trailing newline on the last line -- a `while read` without the
# `|| [ -n "$entry" ]` guard drops it, and the last entry in these lists is a
# real file.
printf '%s\n' \
	'/usr/lib/sensors/libsns_present.so' \
	'# a comment, and an empty line follow' \
	'' \
	'/usr/lib/sensors/libsns_gone.so' \
	'/etc/sensors/broken.ini' > "$ext/scripts/excludes/testsoc_testvariant.list"
printf '%s' '/etc/sensors/present.ini' >> "$ext/scripts/excludes/testsoc_testvariant.list"

out="$work/out.txt"
set +e
env TARGET_DIR="$target" \
	BR2_EXTERNAL_GENERAL_PATH="$ext" \
	BR2_CONFIG="$work/br2config" \
	OPENIPC_SOC_MODEL=testsoc \
	OPENIPC_VARIANT=testvariant \
	bash "$SCRIPT_UNDER_TEST" > "$out" 2>&1
status=$?
set -e

[ "$status" -eq 0 ] && ok "exit status is 0" \
	|| bad "exit status is $status, a stale excludes entry must not break a build"

[ ! -e "$target/usr/lib/sensors/libsns_present.so" ] && ok "present entry was removed" \
	|| bad "present entry survived"
[ ! -e "$target/etc/sensors/present.ini" ] && ok "entry on a line without a trailing newline was removed" \
	|| bad "last entry ignored -- the read loop drops a file with no trailing newline"
[ ! -L "$target/etc/sensors/broken.ini" ] && ok "dangling symlink was removed" \
	|| bad "dangling symlink survived -- the -L arm is missing"

grep -q 'excludes: /usr/lib/sensors/libsns_gone.so matched no file' "$out" \
	&& ok "absent entry is reported" || bad "absent entry was not reported"
grep -q 'excludes: 1 of 4 entries' "$out" \
	&& ok "summary counts 1 stale of 4 real entries" \
	|| bad "summary line wrong or missing: $(grep '^excludes: [0-9]' "$out" || echo '<none>')"
grep -q 'matched no file' "$out" && ! grep -q "excludes: # " "$out" \
	&& ok "comment lines are not treated as paths" || bad "a comment line was treated as a path"

# ----- Part 2: drift -----
# Anchored on the loop, not on the message text, so wording can change freely.
if grep -q 'while IFS= read -r entry' "$SCRIPT_UNDER_TEST" &&
   grep -q 'matched no file' "$SCRIPT_UNDER_TEST"; then
	ok "excludes step still routes the list through a reporting loop"
else
	bad "excludes step no longer reports stale entries -- reverted to a bare xargs?"
fi

echo
if [ "$fail" -eq 0 ]; then
	echo "All excludes-report checks passed."
else
	echo "$fail check(s) failed."
	exit 1
fi
