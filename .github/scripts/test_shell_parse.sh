#!/bin/bash
# Parse-check every shell script that ships inside the firmware image.
#
# Before this, the only parse coverage in CI was two lines in shell-tests.yml
# pinned to sysupgrade. Everything else that lands in the rootfs -- the init
# scripts, /etc/profile sourced by every login, /init for initramfs boards, the
# vendor load_* scripts that insert the SoC drivers -- shipped with nothing
# confirming it even parses. Nothing else covers them: the overlay and each
# package's install step copy these files in verbatim, so a green build says
# nothing about their syntax. A typo's first symptom is a service, or a driver
# load, that silently does not happen on a camera.
#
# DISCOVERY is by shebang over the whole overlay and every package files/ dir,
# not by a list of paths. An earlier draft hand-listed globs (init.d, usr/sbin,
# files/S??) and silently missed 80 of the 127 scripts -- /init, /etc/rc.local,
# the mdev and udhcpc helpers, the network if-up hooks, and every
# osdrv-*/files/script/load_* -- because they did not match the shapes guessed
# at. Anything with a shell shebang is checked wherever it lives, so a script
# added in a new shape is covered when it lands.
#
# INTERPRETER is busybox ash, because that is what runs on the device, and only
# that. dash is deliberately NOT used: the shipped busybox is built with
# CONFIG_ASH_BASH_COMPAT=y (general/package/busybox/busybox.config), so the
# `function name()` keyword is valid in the field, and four scripts that work on
# real hardware -- both hi3516cv6xx/hi3519dv500 load_hisilicon, msc313e
# auto_run.sh, infinity6e zoom.sh -- are rejected by dash. Checking against a
# stricter shell than the target turns working code red, which is worse than no
# check at all: it trains people to ignore the job.
#
# SCOPE: syntax only. `-n` parses without executing, so a passing file may still
# be wrong at runtime, and a bashism that ash accepts is by definition not
# reported here. This is the cheap half of the problem, not a portability audit
# -- see the note in shell-tests.yml about checkbashisms.

set -u

# CI sets STRICT=1: there, a missing busybox means the check silently did not
# happen, which should fail loudly rather than pass a green run. Locally it just
# reports what it could not do and carries on.
STRICT=${STRICT:-0}

fail=0
ok()   { echo "ok   $*"; }
bad()  { echo "FAIL $*"; fail=$((fail + 1)); }
note() { echo "--   $*"; }

BUSYBOX=$(command -v busybox 2>/dev/null || true)

if [ -z "$BUSYBOX" ]; then
	if [ "$STRICT" != 0 ]; then
		echo "FAIL busybox not found and STRICT=$STRICT -- refusing to report a"
		echo "     pass when nothing was actually parsed."
		exit 1
	fi
	note "busybox not found: nothing to parse against, skipping."
	note "install busybox-static to run this as the device would."
	exit 0
fi

# A busybox built without CONFIG_ASH_BASH_COMPAT rejects `function name()`,
# which the shipped one accepts. Running that parser here would fail scripts
# that are correct on hardware, so refuse rather than report false errors.
if ! "$BUSYBOX" ash -c 'function _probe() { :; }' 2>/dev/null; then
	echo "FAIL this busybox lacks ASH_BASH_COMPAT, so it rejects the 'function'"
	echo "     keyword that the shipped busybox accepts. Refusing to apply a"
	echo "     stricter parser than the device runs."
	exit 1
fi

# Everything the overlay drops into the rootfs, plus every file a package
# installs from its files/ dir. Selection is by shebang below rather than by
# path, so nothing depends on guessing where scripts live. Symlinks are resolved
# and the list deduplicated: usr/sbin has five aliases pointing at extutils, and
# parsing it six times only pads the count.
candidates() {
	{
		find general/overlay \( -type f -o -type l \) 2>/dev/null
		find general/package -path '*/files/*' \( -type f -o -type l \) 2>/dev/null
	} | while IFS= read -r p; do
		[ -f "$p" ] || continue
		realpath --relative-to=. "$p" 2>/dev/null || echo "$p"
	done | sort -u
}

err=$(mktemp)
trap 'rm -f "$err"' EXIT

checked=0
skipped=0

while IFS= read -r f; do
	[ -n "$f" ] && [ -f "$f" ] || continue

	# /etc/profile is sourced by the login shell and carries no shebang; every
	# other candidate has to declare itself.
	if [ "$f" = general/overlay/etc/profile ]; then
		kind=ash
	else
		case "$(head -1 "$f" 2>/dev/null)" in
			'#!'*bash*) kind=bash ;;
			'#!'*sh*)   kind=ash ;;
			*)          continue ;;   # not a shell script: config, blob, doc
		esac
	fi

	checked=$((checked + 1))

	if [ "$kind" = bash ]; then
		if bash -n "$f" 2>"$err"; then
			ok "bash $f"
		else
			bad "bash $f"
			sed 's/^/       /' "$err"
		fi
		continue
	fi

	if "$BUSYBOX" ash -n "$f" 2>"$err"; then
		ok "ash  $f"
	else
		bad "ash  $f"
		sed 's/^/       /' "$err"
	fi
done <<EOF
$(candidates)
EOF

echo
echo "checked $checked shell script(s)"

# A discovery bug that quietly matches nothing would otherwise look identical to
# a clean run, which is how the previous version of this file hid its blind spot.
if [ "$checked" -lt 100 ]; then
	echo "FAIL only $checked script(s) discovered; the tree holds ~127, so"
	echo "     discovery has regressed. Check the find paths above."
	exit 1
fi

if [ "$fail" -ne 0 ]; then
	echo "$fail check(s) failed"
	exit 1
fi

echo "all parsed clean under busybox ash"
