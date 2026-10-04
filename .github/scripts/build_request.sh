#!/bin/sh
# Apply a firmware-explorer build request to a defconfig, and check afterwards
# what the build actually did with it.
#
#   build_request.sh apply  <issue-body> <defconfig> <symbols-out>
#   build_request.sh verify <symbols>    <dot-config>
#
# The explorer (openipc.org/firmware-explorer, "What if") files an issue whose
# body carries a "## Defconfig fragment" section with one fenced block of
# `# BR2_... is not set` lines. build-one.yml takes that issue's number,
# appends the block to the board's defconfig, and builds.
#
# The issue body is written by whoever filed it, so it is data, never code:
# `apply` accepts only lines of exactly the form `# BR2_<NAME> is not set`,
# ignores blank lines and other `#` comments, and refuses the whole request on
# anything else. A request can therefore only switch symbols off -- it can
# never enable a package, set a string, or reach the shell.
#
# `verify` exists because switching a symbol off is a request, not a command:
# a package that `select`s it turns it back on, and Kconfig says nothing. It
# prints every requested symbol that is still `=y` in the final .config, so
# the release notes and the reply on the issue say which parts of the request
# the build could not honour.

set -eu

die() { echo "build_request: $*" >&2; exit 1; }

apply() {
	body=$1 defconfig=$2 out=$3
	[ -f "$body" ] || die "no issue body at $body"
	[ -f "$defconfig" ] || die "no defconfig at $defconfig"

	# The first fenced block after the "## Defconfig fragment" heading. CRs are
	# dropped: an issue edited in a browser comes back with CRLF line ends.
	block=$(tr -d '\r' < "$body" | awk '
		/^## Defconfig fragment[[:space:]]*$/ { seen = 1; next }
		seen && !open && /^```/ { open = 1; next }
		open && /^```/ { exit }
		open { print }
	')
	[ -n "$block" ] || die "no fenced block under '## Defconfig fragment'"

	: > "$out"
	printf '%s\n' "$block" | while IFS= read -r line; do
		case "$line" in
			"") ;;
			*)
				if printf '%s\n' "$line" | grep -Eq '^# BR2_[A-Z0-9_]+ is not set$'; then
					printf '%s\n' "$line" | sed -E 's/^# (BR2_[A-Z0-9_]+) is not set$/\1/' >> "$out"
				elif printf '%s\n' "$line" | grep -Eq 'is not set|^#[[:space:]]*[A-Za-z0-9_]*_[A-Za-z0-9_$(]'; then
					# Looks like a symbol line but is not the one accepted
					# form: a mangled or tampered request, not a comment.
					die "refusing malformed symbol line: $line"
				elif printf '%s\n' "$line" | grep -q '^#'; then
					:
				else
					die "refusing line that is not '# BR2_<NAME> is not set': $line"
				fi
				;;
		esac
	done

	[ -s "$out" ] || die "the fragment names no symbol to disable"
	sort -u -o "$out" "$out"

	{
		echo ""
		echo "# Build request: symbols switched off by the firmware explorer."
		sed 's/^\(.*\)$/# \1 is not set/' "$out"
	} >> "$defconfig"
	echo "build_request: switching off $(wc -l < "$out") symbol(s) in ${defconfig}:"
	sed 's/^/  /' "$out"
}

verify() {
	symbols=$1 config=$2
	[ -f "$symbols" ] || die "no symbol list at $symbols"
	[ -f "$config" ] || die "no .config at $config"
	while IFS= read -r sym; do
		if grep -q "^${sym}=" "$config"; then
			echo "$sym"
		fi
	done < "$symbols"
}

case "${1:-}" in
	apply)  [ $# -eq 4 ] || die "usage: $0 apply <issue-body> <defconfig> <symbols-out>"; apply "$2" "$3" "$4" ;;
	verify) [ $# -eq 3 ] || die "usage: $0 verify <symbols> <dot-config>"; verify "$2" "$3" ;;
	*) die "usage: $0 apply|verify ..." ;;
esac
