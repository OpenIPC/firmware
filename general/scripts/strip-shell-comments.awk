# Drop whole-line comments and blank lines from a POSIX shell script.
#
# Comments are worth writing and worth keeping in the tree, but every byte of
# them is also flashed to an 8MB camera. sysupgrade alone reached 52KB, 57% of
# it comment, and tipped hi3519v101_lite 4KB past its 5120KB rootfs cap on
# 2026-08-18. Stripping at build time keeps the prose in git and off the flash.
#
# Only WHOLE-LINE comments go. A trailing `foo # bar` is left alone: deciding
# where the code ends needs the same care as the cases below, for a few bytes.
#
# Three things look like a comment line and are not, so each is tracked:
#
#   heredoc bodies      a `#` there is data the script writes out
#   multi-line strings  VAR="...\n# still the string\n..."
#   continuations       `foo \` joins the next line onto this one, so a
#                       following `#...` is an argument, not a comment
#
# Quote state is carried across lines, which is what makes those distinctions
# possible; a naive `/^[[:space:]]*#/d` gets all three wrong.

# Walk the line, tracking quotes, and return the part of it that is code.
# Sets `cont` when the line ends in an unescaped backslash.
function code_of(line,   n, i, ch, prev) {
	n = length(line)
	i = 1
	cont = 0
	while (i <= n) {
		ch = substr(line, i, 1)

		if (sq) {                          # '...' — nothing escapes
			if (ch == "'") sq = 0
			i++
			continue
		}
		if (dq) {                          # "..." — backslash escapes
			if (ch == "\\") { i += 2; continue }
			if (ch == "\"") dq = 0
			i++
			continue
		}

		if (ch == "\\") {
			if (i == n) { cont = 1; return line }
			i += 2
			continue
		}
		if (ch == "'")  { sq = 1; i++; continue }
		if (ch == "\"") { dq = 1; i++; continue }
		if (ch == "#") {
			# `#` opens a comment only at the start of a word.
			prev = (i == 1) ? "" : substr(line, i - 1, 1)
			if (i == 1 || prev ~ /[ \t;&|()<>]/) return substr(line, 1, i - 1)
		}
		i++
	}
	return line
}

# Queue every heredoc opened on this line, in the order the shell reads them.
function collect_heredocs(code,   pos, seg, at, m, delim) {
	pos = 1
	while (1) {
		seg = substr(code, pos)
		if (!match(seg, /<<-?[ \t]*("[^"]+"|'[^']+'|[A-Za-z_][A-Za-z0-9_]*)/)) return
		at = pos + RSTART - 1                 # where the `<<` sits in `code`
		m = substr(code, at, RLENGTH)
		pos = at + RLENGTH
		# `<<<` is a herestring, with no terminator line to wait for. The match
		# lands on its LAST two `<`, so the tell is the character before it.
		if (at > 1 && substr(code, at - 1, 1) == "<") continue
		delim = m
		sub(/^<<-?[ \t]*/, "", delim)
		gsub(/^["']|["']$/, "", delim)
		nd++
		dstack[nd] = delim
		dtab[nd] = (substr(m, 3, 1) == "-")   # `<<-` also un-tabs the terminator
	}
}

BEGIN { nd = 0; sq = 0; dq = 0; cont = 0; prev_cont = 0 }

{
	# Inside a heredoc body: verbatim until the terminator.
	if (nd > 0) {
		print
		probe = $0
		if (dtab[1]) sub(/^\t+/, "", probe)
		if (probe == dstack[1]) {
			for (i = 1; i < nd; i++) { dstack[i] = dstack[i + 1]; dtab[i] = dtab[i + 1] }
			nd--
		}
		next
	}

	# The shebang is the one `#` line that does work.
	if (NR == 1 && /^#!/) { print; next }

	# Mid-string or mid-continuation, this line is not ours to read.
	if (sq || dq || prev_cont) {
		print
		code = code_of($0)
		prev_cont = cont
		collect_heredocs(code)
		next
	}

	if (/^[ \t]*$/) next

	code = code_of($0)
	if (code ~ /^[ \t]*$/) next          # the line was only a comment

	print
	prev_cont = cont
	collect_heredocs(code)
}
