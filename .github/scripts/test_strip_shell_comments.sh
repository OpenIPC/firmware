#!/bin/bash
# Tests for general/scripts/strip-shell-comments.awk, the build-time pass that
# drops whole-line comments from every shell script before it is flashed.
#
# The transform runs on 129 scripts that nobody re-reads after the build, so a
# wrong edit here is a corrupted rootfs that still builds green: a mangled
# load_hisilicon is a camera that boots to no video, and the comments are gone
# from the shipped copy, so the file on the device does not even read like the
# one in git. Cheap to test, expensive to get wrong.
#
# Three constructs look like a comment line and are not, and each is a way to
# silently change what a script DOES:
#
#   heredoc bodies      the `#` is data the script writes out
#   multi-line strings  VAR="...\n# still the string\n..."
#   continuations       `foo \` splices the next line on, so a following
#                       `#...` is an argument, not a comment
#
# Part 1 pins each of those with a fixture. Part 2 is the broader claim -- that
# stripping changes no shipped script's syntax -- checked by parsing all of them
# before and after with the busybox ash the device runs.

set -u

STRICT=${STRICT:-0}

AWK_SCRIPT=${AWK_SCRIPT:-general/scripts/strip-shell-comments.awk}

fail=0
ok()   { echo "ok   $*"; }
bad()  { echo "FAIL $*"; fail=$((fail + 1)); }
note() { echo "--   $*"; }

[ -f "$AWK_SCRIPT" ] || { echo "FAIL cannot find $AWK_SCRIPT — run me from the repo root"; exit 1; }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

strip() { awk -f "$AWK_SCRIPT" "$1"; }

# ---------------------------------------------------------------------------
# Part 1 — the constructs a naive `/^[[:space:]]*#/d` gets wrong.
# ---------------------------------------------------------------------------

cat > "$WORK/fixture.sh" <<'FIXTURE'
#!/bin/sh
# GONE plain comment
    # GONE indented comment
echo one   # LIVE trailing comment rides along with its code line
cat <<EOF
# LIVE heredoc body
EOF
cat <<-'QUOTED'
	# LIVE tab-heredoc body
	QUOTED
VAR="first
# LIVE inside a double-quoted string
last"
SQ='first
# LIVE inside a single-quoted string
last'
echo two \
# LIVE spliced onto the continuation above
echo three
FIXTURE

strip "$WORK/fixture.sh" > "$WORK/fixture.out"

if grep -q GONE "$WORK/fixture.out"; then
	bad "a plain comment line survived stripping"
else
	ok "plain and indented comment lines are dropped"
fi

live=$(grep -c LIVE "$WORK/fixture.out")
if [ "$live" = 6 ]; then
	ok "heredoc bodies, multi-line strings and continuations are left alone (6/6)"
else
	bad "expected 6 LIVE lines to survive, got $live"
	diff "$WORK/fixture.sh" "$WORK/fixture.out"
fi

if [ "$(head -1 "$WORK/fixture.out")" = '#!/bin/sh' ]; then
	ok "the shebang survives"
else
	bad "the shebang was stripped — every script would lose its interpreter"
fi

# A herestring has no terminator line. Mistaking it for a heredoc would make the
# stripper swallow the entire rest of the file as an unterminated body.
cat > "$WORK/herestring.sh" <<'HERESTRING'
#!/bin/bash
grep x <<<"inline"
# GONE after a herestring
echo end
HERESTRING

strip "$WORK/herestring.sh" > "$WORK/herestring.out"
if grep -q GONE "$WORK/herestring.out"; then
	bad "<<< was treated as a heredoc: stripping stopped at the herestring"
elif grep -q "echo end" "$WORK/herestring.out"; then
	ok "<<< is read as a herestring, not a heredoc"
else
	bad "content after a herestring was lost"
fi

# Two heredocs opened on one line are consumed in order; getting the order wrong
# ends the first body at the second's terminator.
cat > "$WORK/twoheredoc.sh" <<'TWO'
#!/bin/sh
cat <<A - <<B
# LIVE body one
A
# LIVE body two
B
# GONE trailing comment
TWO

strip "$WORK/twoheredoc.sh" > "$WORK/twoheredoc.out"
if [ "$(grep -c LIVE "$WORK/twoheredoc.out")" = 2 ] && ! grep -q GONE "$WORK/twoheredoc.out"; then
	ok "two heredocs on one line are consumed in order"
else
	bad "stacked heredocs on one line were mis-tracked"
fi

# Running twice must equal running once: the build re-runs post-build hooks on
# an existing target dir, so a non-idempotent pass would eat code the second time.
strip "$WORK/fixture.out" > "$WORK/fixture.twice"
if cmp -s "$WORK/fixture.out" "$WORK/fixture.twice"; then
	ok "stripping is idempotent"
else
	bad "stripping twice differs from stripping once"
	diff "$WORK/fixture.out" "$WORK/fixture.twice"
fi

# ---------------------------------------------------------------------------
# Part 2 — no shipped script changes syntax. This is the claim that matters;
# Part 1 only explains the ways it could break.
# ---------------------------------------------------------------------------

BUSYBOX=$(command -v busybox 2>/dev/null || true)

if [ -z "$BUSYBOX" ]; then
	if [ "$STRICT" != 0 ]; then
		echo "FAIL busybox not found and STRICT=$STRICT -- refusing to report a"
		echo "     pass when no shipped script was actually checked."
		exit 1
	fi
	note "busybox not found: skipping the shipped-script sweep."
	note "install busybox-static to run this as the device would."
	[ "$fail" -eq 0 ] && echo && echo "Fixture checks passed (sweep skipped)."
	exit "$([ "$fail" -eq 0 ] && echo 0 || echo 1)"
fi

# Same bar as test_shell_parse.sh: the shipped busybox sets ASH_BASH_COMPAT, so
# a busybox without it would reject `function name()` and fail scripts that are
# correct on hardware.
if ! "$BUSYBOX" ash -c 'function _probe() { :; }' 2>/dev/null; then
	echo "FAIL this busybox lacks ASH_BASH_COMPAT, so it rejects the 'function'"
	echo "     keyword that the shipped busybox accepts."
	exit 1
fi

candidates() {
	{
		find general/overlay -type f 2>/dev/null
		find general/package -path '*/files/*' -type f 2>/dev/null
	} | sort -u
}

checked=0
drifted=0
saved=0

while IFS= read -r f; do
	[ -n "$f" ] && [ -f "$f" ] || continue

	# Skip binaries before reading a line of them: package files/ dirs carry
	# blobs, and their NUL bytes make the shebang test below noisy.
	grep -Iq . "$f" 2>/dev/null || continue

	case "$(head -1 "$f" 2>/dev/null)" in
		'#!'*sh*) ;;
		*) [ "$f" = general/overlay/etc/profile ] || continue ;;
	esac

	strip "$f" > "$WORK/stripped" 2>/dev/null || {
		bad "awk failed on $f"
		drifted=$((drifted + 1))
		continue
	}

	"$BUSYBOX" ash -n "$f" 2>/dev/null;            before=$?
	"$BUSYBOX" ash -n "$WORK/stripped" 2>/dev/null; after=$?

	if [ "$before" != "$after" ]; then
		bad "$f parses differently after stripping (before=$before after=$after)"
		drifted=$((drifted + 1))
		continue
	fi

	if [ "$(head -c2 "$WORK/stripped")" != '#!' ] && [ "$f" != general/overlay/etc/profile ]; then
		bad "$f lost its shebang"
		drifted=$((drifted + 1))
		continue
	fi

	checked=$((checked + 1))
	saved=$((saved + $(wc -c < "$f") - $(wc -c < "$WORK/stripped")))
done <<EOF
$(candidates)
EOF

if [ "$checked" -eq 0 ]; then
	bad "no shipped scripts were found — discovery is broken, not the tree"
elif [ "$drifted" -eq 0 ]; then
	ok "$checked shipped scripts parse identically after stripping (${saved} bytes saved)"
fi

echo
if [ "$fail" -eq 0 ]; then
	echo "All strip-shell-comments checks passed."
	exit 0
fi
echo "$fail check(s) failed."
exit 1
