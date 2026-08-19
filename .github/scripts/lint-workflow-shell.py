#!/usr/bin/env python3
"""Parse-check every `run:` block in .github/workflows/.

WHY THIS EXISTS. The publish job in build.yml carries
`if: github.event_name != 'pull_request'` -- there are no artifacts to publish
on a PR, so the whole path is unreachable from PR CI by construction. Every
edit to it ships unexecuted, and the first thing to run it is the 22:30 cron.

That is not hypothetical. OpenIPC/builder runs the same publish job, ported
from this one, and OpenIPC/builder#121 edited its Collect step and shipped an
`if` with no closing `fi`. Both PR runs were green without ever reaching the
step. The nightly then built all 107 devices, staged 219 assets, and died in
`bash -e` with "syntax error: unexpected end of file" before writing a single
release. Nothing published that night.

The guard that broke there is the one this repo backported in #2279, so the
same typo is one edit away here, in a job whose failures surface at 22:30 to
nobody. This is the cheap check that closes the gap: no runners, no artifacts,
no matrix, and it fails in seconds. Ported from OpenIPC/builder#122.

SCOPE is syntax only, deliberately. `-n` parses without executing, so a block
that passes here can still be wrong at runtime, and this says nothing about
quoting, unset variables, or the `-e` semantics that bite in these scripts. A
shellcheck pass over the same blocks is a strictly bigger change (it wants
per-block disable lists for the ${{ }} substitution below) and is worth doing
separately rather than smuggling in behind a syntax fix.

DISCOVERY is a mapping with a scalar `run:` that came out of a `steps:`
sequence -- the schema invariant that makes something a step, and no more than
that. Hardcoding jobs.*.steps[*] would quietly miss anything that moves, and a
composite action's `runs: steps:` is covered here by the same rule. Requiring
the `steps:` parent is not decoration: without it an action input, an env var
or a matrix field called `run` gets parsed as shell and fails a workflow that
is fine. Silently checking nothing looks exactly like a clean run, which is the
failure mode this file exists to prevent, so there is a floor check at the end.

EXPRESSIONS. `${{ ... }}` is not shell and cannot be parsed as shell, so each
one is replaced with a plain word before the check. Finding where one ends is
scanned rather than regexed, because `.*?\\}\\}` stops at the first `}}` even
when it sits inside a string literal -- `${{ fromJSON('{"a": {"b": 1}}') }}`
gets cut mid-literal and the leftover `') }}` then fails as an unbalanced
quote. The substitution is still a real limitation: an expression that
interpolates something with shell syntax in it -- a matrix value holding
`foo && bar`, say -- is checked as the word, not as what it expands to. It is
the same limitation `bash -n` has with any variable. Line counts are preserved
so reported line numbers still point at the right line of the workflow, and
--self-test asserts the substitution did not neuter the check.

Usage:
    lint-workflow-shell.py               # check .github/workflows/
    lint-workflow-shell.py PATH...       # check specific files
    lint-workflow-shell.py --self-test   # check this checker still catches things
"""

import glob
import subprocess
import sys
import tempfile
from pathlib import Path

try:
    import yaml
except ImportError:  # pragma: no cover - depends on the host, not the tree
    sys.exit(
        "lint-workflow-shell: PyYAML is required and missing.\n"
        "  CI installs it (see .github/workflows/lint.yml); locally:\n"
        "    apt install python3-yaml   # or: pip install pyyaml"
    )

# Steps with no `shell:` run under bash on a Linux runner; GitHub's default is
# `bash -e {0}`. The -e changes runtime behaviour, never parsing, so -n alone is
# the right check for both.
DEFAULT_SHELL = "bash"

# How the `shell:` value maps to a parser. Anything not listed is skipped and
# reported, so a step that starts using `python` or `pwsh` shows up as a gap
# rather than being silently dropped.
PARSERS = {
    "bash": ["bash", "-n"],
    "sh": ["sh", "-n"],
}

EXPR_OPEN = "${{"

# The floor below is a discovery guard, not a target: it only has to catch "the
# walk broke and found nothing", which is the case that would otherwise report
# green. Deliberately well under the current count (43 across the eleven
# workflows) so ordinary churn never trips it, and only applied to a full-tree
# scan -- checking one file by hand legitimately finds fewer.
MIN_BLOCKS = 25


def end_of_expression(text, start):
    """Index just past the `}}` closing the expression opened at `start`.

    Scanned rather than matched with `\\$\\{\\{.*?\\}\\}`, because that stops at
    the first `}}` even when it is inside a quoted string:
    `${{ fromJSON('{"a": {"b": 1}}') }}` would be cut mid-literal, leaving
    `') }}` behind to be parsed as shell and failing a workflow that is fine.
    GitHub string literals are single-quoted and escape a quote by doubling it.

    Returns None if the expression is never closed, in which case the caller
    leaves the text alone -- that is a malformed workflow, not our business.
    """
    i = start + len(EXPR_OPEN)
    quote = None
    while i < len(text):
        c = text[i]
        if quote is not None:
            if c == quote:
                if i + 1 < len(text) and text[i + 1] == quote:
                    i += 2          # '' is an escaped quote, still inside
                    continue
                quote = None
            i += 1
        elif c in "'\"":
            quote = c
            i += 1
        elif text.startswith("}}", i):
            return i + 2
        else:
            i += 1
    return None


def substitute(text):
    """Replace every ${{ }} with a word, preserving the line count.

    Line numbers have to survive or the errors point at nothing. A multi-line
    expression is replaced by the word plus the newlines it spanned.
    """
    out = []
    i = 0
    while True:
        start = text.find(EXPR_OPEN, i)
        if start < 0:
            out.append(text[i:])
            return "".join(out)
        end = end_of_expression(text, start)
        if end is None:
            out.append(text[i:])
            return "".join(out)
        out.append(text[i:start])
        out.append("__GHA_EXPR__" + "\n" * text.count("\n", start, end))
        i = end


def walk(node, shell, out, in_steps=False):
    """Collect (run_text, shell, line) for every step under `node`.

    `shell` is the default inherited from an enclosing `defaults.run.shell`,
    which is how both workflow-level and job-level defaults reach a step
    without this needing to know where in the document it is.

    `in_steps` says this node came out of a `steps:` sequence, which is what
    makes a mapping a step. An earlier version collected any mapping with a
    scalar `run` key anywhere in the document; that is one key name away from
    linting things that are not shell at all -- an action input or an env var
    or a matrix field called `run` -- and failing a workflow that is fine.
    Requiring the `steps:` parent is still not a hardcoded jobs.*.steps[*]
    path, so composite actions (`runs: steps:`) are covered by the same rule.
    """
    if isinstance(node, yaml.MappingNode):
        keys = {}
        for k, v in node.value:
            if isinstance(k, yaml.ScalarNode):
                keys[k.value] = v

        # A `defaults: run: shell:` here applies to everything below it.
        defaults = keys.get("defaults")
        if isinstance(defaults, yaml.MappingNode):
            for dk, dv in defaults.value:
                if isinstance(dk, yaml.ScalarNode) and dk.value == "run":
                    if isinstance(dv, yaml.MappingNode):
                        for rk, rv in dv.value:
                            if (
                                isinstance(rk, yaml.ScalarNode)
                                and rk.value == "shell"
                                and isinstance(rv, yaml.ScalarNode)
                            ):
                                shell = rv.value

        run = keys.get("run")
        if in_steps and isinstance(run, yaml.ScalarNode):
            step_shell = shell
            sh = keys.get("shell")
            if isinstance(sh, yaml.ScalarNode):
                step_shell = sh.value
            name = keys.get("name")
            label = name.value if isinstance(name, yaml.ScalarNode) else "<unnamed>"
            out.append(
                {
                    "text": run.value,
                    "shell": step_shell,
                    "line": run.start_mark.line + 1,
                    "name": label,
                }
            )

        for k, v in node.value:
            key = k.value if isinstance(k, yaml.ScalarNode) else None
            walk(v, shell, out, in_steps=(key == "steps"))

    elif isinstance(node, yaml.SequenceNode):
        # A sequence does not change what its items are; `steps:` points at the
        # sequence, and its items are the steps.
        for v in node.value:
            walk(v, shell, out, in_steps)


def parse_error(text, shell):
    """Return bash's complaint about `text`, or None if it parses."""
    # `shell: bash` and `shell: bash {0}` and `shell: bash --noprofile {0}` all
    # mean bash; take the program name and ignore the argument template.
    prog = (shell or DEFAULT_SHELL).split()[0]
    argv = PARSERS.get(prog)
    if argv is None:
        return ("skip", prog)

    body = substitute(text)
    with tempfile.NamedTemporaryFile("w", suffix=".sh", delete=False) as f:
        f.write(body)
        tmp = f.name
    try:
        p = subprocess.run(
            argv + [tmp], capture_output=True, text=True, timeout=30
        )
        if p.returncode == 0:
            return None
        # Rewrite the temp path out of the message; the caller adds the real
        # location. Keep the shell's own line number, it is the offset into the
        # block and the substitution preserved it.
        return ("fail", p.stderr.replace(tmp, "<run block>").strip())
    finally:
        Path(tmp).unlink(missing_ok=True)


def check(paths, enforce_floor):
    blocks = 0
    failures = 0
    skipped = []

    for path in paths:
        text = Path(path).read_text()
        try:
            root = yaml.compose(text)
        except yaml.YAMLError as e:
            print(f"FAIL {path}: not parseable as YAML: {e}")
            failures += 1
            continue
        if root is None:
            continue

        found = []
        walk(root, DEFAULT_SHELL, found)

        for b in found:
            blocks += 1
            where = f"{path}:{b['line']} ({b['name']})"
            result = parse_error(b["text"], b["shell"])
            if result is None:
                print(f"ok   {where}")
            elif result[0] == "skip":
                skipped.append(f"{where} [shell: {result[1]}]")
            else:
                print(f"FAIL {where}")
                for line in result[1].splitlines():
                    print(f"       {line}")
                failures += 1

    print()
    if skipped:
        print(f"skipped {len(skipped)} block(s) in a shell this does not parse:")
        for s in skipped:
            print(f"  -- {s}")
        print()

    print(f"checked {blocks} run block(s)")

    # A walk that matched nothing is indistinguishable from a clean run unless
    # something asserts it found the work. See the module docstring.
    if enforce_floor and blocks < MIN_BLOCKS:
        print(
            f"FAIL only {blocks} run block(s) discovered, expected at least "
            f"{MIN_BLOCKS}; the walk has regressed rather than the workflows "
            f"having shrunk that far."
        )
        return 1

    if failures:
        print(f"{failures} block(s) failed to parse")
        return 1

    print("all run blocks parse clean")
    return 0


def self_test():
    """Prove the checker still rejects what it is supposed to reject.

    The substitution in particular is easy to break in a way that makes
    everything pass -- replace ${{ }} with something that swallows the rest of
    the line and every block parses forever after.
    """
    ok = True

    def expect(name, text, shell, should_fail):
        nonlocal ok
        got = parse_error(text, shell)
        failed = got is not None and got[0] == "fail"
        if failed != should_fail:
            want = "reject" if should_fail else "accept"
            print(f"FAIL self-test: expected to {want} {name}, got {got!r}")
            ok = False
        else:
            print(f"ok   self-test: {name}")

    # The exact shape that shipped in OpenIPC/builder#121: an if with no fi.
    expect(
        "unterminated if",
        'if [ "${{ needs.x.result }}" = "success" ]; then\n  echo hi\n',
        "bash",
        True,
    )
    # The same block, closed. If the substitution is broken this fails too, and
    # the pair together is what makes the check meaningful.
    expect(
        "terminated if with an expression in it",
        'if [ "${{ needs.x.result }}" = "success" ]; then\n  echo hi\nfi\n',
        "bash",
        False,
    )
    expect("unbalanced quote", 'echo "unterminated\n', "bash", True)
    expect("plain block", "echo hello\n", None, False)

    # A `}}` inside an expression's string literal must not end the expression.
    # A regex stopping at the first `}}` leaves `') }}` behind, which is an
    # unbalanced quote, so this block used to fail while being perfectly valid.
    expect(
        "expression whose string literal contains }}",
        "x=${{ fromJSON('{\"a\": {\"b\": 1}}') }}\necho \"$x\"\n",
        "bash",
        False,
    )

    # An expression spanning lines must not shift the reported line number.
    shifted = substitute("a\n${{ foo\n  .bar }}\nc\n")
    if shifted.count("\n") != 4:
        print("FAIL self-test: substitution changed the line count")
        ok = False
    else:
        print("ok   self-test: substitution preserves line count")

    # Only things in a `steps:` sequence are steps. An action input, an env
    # var or a matrix field called `run` holds arbitrary text, and linting it
    # as shell fails workflows that are fine.
    doc = yaml.compose(
        "jobs:\n"
        "  j:\n"
        "    env:\n"
        "      run: not shell 'at all\n"
        "    steps:\n"
        "      - name: real\n"
        "        run: echo hi\n"
        "      - uses: some/action@v1\n"
        "        with:\n"
        "          run: also not shell 'at all\n"
    )
    found = []
    walk(doc, DEFAULT_SHELL, found)
    names = sorted(b["name"] for b in found)
    if names != ["real"]:
        print(f"FAIL self-test: expected only the real step, collected {names}")
        ok = False
    else:
        print("ok   self-test: only steps under steps: are collected")

    print()
    if not ok:
        print("self-test failed")
        return 1
    print("self-test passed")
    return 0


def main(argv):
    if "--self-test" in argv:
        return self_test()

    paths = [a for a in argv if not a.startswith("-")]
    # The discovery floor only means anything over the whole tree; an explicit
    # file list is someone checking one thing on purpose.
    full_scan = not paths
    if full_scan:
        paths = sorted(
            glob.glob(".github/workflows/*.yml")
            + glob.glob(".github/workflows/*.yaml")
        )
    if not paths:
        print("FAIL no workflow files found; run this from the repo root.")
        return 1
    return check(paths, enforce_floor=full_scan)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
