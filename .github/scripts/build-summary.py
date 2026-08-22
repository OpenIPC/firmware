#!/usr/bin/env python3
"""Render the firmware matrix into one readable summary, per run and over time.

WHY THIS EXISTS. build.yml builds ~99 boards with `fail-fast: false`, so one
failing cell does not cancel the others -- but the run still reports
`conclusion: failure`. Issue #2271 asked whether that is the signal we want.
The answer, recorded in the ci-gate comment in build.yml, is yes: the nightly's
product is a COMPLETE set of images, and a board that failed leaves `nightly`
and `latest` holding yesterday's image for it, which is stale firmware for
whoever flashes that board. The colour is right.

What was wrong was that the colour was the only output. On 2026-08-12 (run
31649720825) a majestic-webui fixup did a bare `rm` on two files upstream had
deleted, and 94 of 99 boards failed in the same place -- a fact that took
opening 94 job logs to establish, because the run said, in full, "failure".
"The nightly is red" does not distinguish one broken board from ninety-four.
This file is that distinction.

It also reports the class the retry loop hides. Seven attempts with backoff to
1200s absorb most transients (#2036), and a green run says nothing about how
close it came. "Three boards needed retries tonight, all opus-1.4/downloaded"
is the leading indicator that the nightly is about to go red again, and until
now nothing surfaced it.

WHERE THE DATA COMES FROM. Nothing is stored. Three read-only API endpoints:

  * /actions/runs/{id}/jobs -- job names are `Firmware (<board>)`, so the board
    parses out of the name, and steps[] says which step failed, which covers
    the post-build steps for free.
  * /check-runs/{job_id}/annotations -- the classes, attempt counts and
    headroom warnings live here, because build.yml emits them as ::error:: and
    ::warning::. This relies on a job id being usable as a check run id, which
    is true today and is verified against real runs, but is not documented as a
    guarantee. A 404 here degrades that board to "no detail", never to a crash.
  * /actions/workflows/build.yml/runs?event=schedule -- the streak window.
    Scheduled runs always build the full matrix (the selector only narrows
    pull_request), so the window is comparable board-for-board.

Only annotations matching the grammars below are read. That is also what filters
the Node.js-deprecation warning every job carries: noise is excluded by
construction rather than by a denylist that would need maintaining.

COST. ~120 requests for a nightly: 2 for the jobs, one per board for the
annotations, two per historical run. The repo's GITHUB_TOKEN budget is 1000
requests/hour and the publish job already spends ~400 of it on paced asset
uploads, so this is not free and the footer prints the count it actually used.
--no-flakes drops the annotation fetch for boards that PASSED, which is the
bulk of it, if that ever needs reclaiming.

THIS MUST NEVER FAIL THE RUN. It reports; it does not judge. A bug here turning
a good nightly red would be precisely the mistake #2271 decided not to make, so
the workflow step swallows a non-zero exit into a warning and the summary job
is deliberately not in ci-gate's `needs`. What keeps it honest instead is
--self-test, which lint.yml runs as a merge gate: it checks that the annotation
grammars below still match the strings build.yml emits, because a reworded
annotation silently reduces this file to reporting nothing, and reporting
nothing looks exactly like a clean nightly.

Usage:
    build-summary.py --run-id 31649720825 --event schedule
    build-summary.py --run-id 31649720825 --out -      # stdout, for a dry run
    build-summary.py --self-test                       # no network
"""

import argparse
import json
import os
import re
import sys
import threading
import time
import urllib.error
import urllib.parse
import urllib.request
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
WORKFLOW = os.path.join(REPO_ROOT, ".github", "workflows", "build.yml")

API = "https://api.github.com"
# Two weeks of nightlies. Long enough that "red three nights running" is
# visible and a one-off is obviously a one-off, short enough that the window
# costs ~28 requests.
HISTORY_RUNS = 14
# The matrix job's display name. Anything else in the run -- Select boards,
# Preflight, Publish releases, CI Gate, this job -- is not a board.
JOB_NAME = re.compile(r"^Firmware \((?P<board>[^)]+)\)$")

# --------------------------------------------------------------------------
# Annotation grammars
#
# Each entry is (name, regex, anchor). The anchor is a literal substring that
# must still be present in build.yml; --self-test asserts it, so rewording an
# annotation fails a merge gate instead of silently emptying this report.
# --------------------------------------------------------------------------

GRAMMARS = [
    # echo "::error::<board>: build failed after <n> attempts — <cause>"
    ("exhausted",
     re.compile(r"^(?P<board>\S+): build failed after (?P<attempts>\d+) attempts — (?P<cause>.+)$"),
     "build failed after ${attempt} attempts — "),
    # echo "::warning::<board>: attempt <n> failed (<cause>), retrying after <s>s"
    ("retry",
     re.compile(r"^(?P<board>\S+): attempt (?P<attempt>\d+) failed \((?P<cause>.*)\), "
                r"retrying after (?P<sleep>\d+)s$"),
     "attempt ${attempt} failed (${cause}), retrying after "),
    # echo "::error::<board>: -- size exceeded by: ... — the image does not fit ..."
    ("size",
     re.compile(r"^(?P<board>\S+): -- size exceeded by: (?P<detail>.*?) — the image does not fit"),
     "-- size exceeded by:"),
    # echo "::warning::<board>: <image> has <n>KB left of <n>KB"  (from the
    # Makefile's "-- headroom warning:" line, prefix stripped)
    ("headroom",
     re.compile(r"^(?P<board>\S+): (?P<image>\S+) has (?P<left>\d+)KB left of (?P<cap>\d+)KB$"),
     "-- headroom warning: "),
]

# Annotations build.yml emits that are deliberately NOT summarised: they are
# sidecar-generation hiccups that cost a size report or a kconfig graph and
# nothing that reaches an image. Listed rather than ignored silently so the
# floor check below still accounts for every annotation in the job.
IGNORED_ANCHORS = [
    "size-report failed for ",
    "kconfiglib install failed for ",
    "kconfig-graph failed for ",
]

PASS, FAIL, ABSENT = "✓", "✗", "·"


# --------------------------------------------------------------------------
# API client
# --------------------------------------------------------------------------

class ApiError(Exception):
    pass


class Api:
    """Read-only GitHub REST client that counts what it spends."""

    def __init__(self, token, repo, sleep=time.sleep):
        self.token = token
        self.repo = repo
        self.calls = 0
        self._sleep = sleep
        # get() runs on up to eight worker threads, and `self.calls += 1` is
        # load-add-store rather than one atomic step, so concurrent increments
        # can be lost. An under-reported count is the one wrong answer this
        # counter must not give: its whole job is to make the API budget
        # measured rather than assumed.
        self._counting = threading.Lock()

    def get(self, path, params=None):
        url = f"{API}{path}"
        if params:
            url += "?" + urllib.parse.urlencode(params)
        # Six attempts covers a secondary-rate-limit pause; the same shape the
        # publish job's gh_retry uses, for the same reason.
        delay = 5
        for attempt in range(1, 7):
            request = urllib.request.Request(url, headers={
                "Authorization": f"Bearer {self.token}",
                "Accept": "application/vnd.github+json",
                "X-GitHub-Api-Version": "2022-11-28",
            })
            with self._counting:
                self.calls += 1
            try:
                with urllib.request.urlopen(request, timeout=30) as response:
                    return json.load(response)
            except urllib.error.HTTPError as error:
                # 404 is an answer, not a flake: a job with no annotations, or
                # a check run that cannot be addressed by its job id.
                if error.code == 404:
                    raise ApiError(f"404 {path}") from error
                if error.code not in (403, 429, 500, 502, 503, 504) or attempt == 6:
                    raise ApiError(f"{error.code} {path}") from error
                retry_after = error.headers.get("Retry-After")
                self._sleep(int(retry_after) if retry_after else delay)
                delay *= 2
            except urllib.error.URLError as error:
                if attempt == 6:
                    raise ApiError(f"{error} {path}") from error
                self._sleep(delay)
                delay *= 2
        raise ApiError(f"unreachable {path}")

    def paginate(self, path, params, key):
        params = dict(params or {})
        params["per_page"] = 100
        page, out = 1, []
        while True:
            params["page"] = page
            batch = self.get(path, params)[key]
            out += batch
            if len(batch) < 100:
                return out
            page += 1
            # A run with more than 1000 jobs is not a matrix, it is a bug.
            if page > 10:
                return out


# --------------------------------------------------------------------------
# Parsing
# --------------------------------------------------------------------------

def parse_annotations(messages):
    """Bucket a job's annotation messages by grammar, dropping everything else."""
    hits = defaultdict(list)
    for message in messages:
        # Annotations arrive with the ::level:: marker already stripped, and
        # multi-line ones only ever match on their first line.
        line = (message or "").strip().splitlines()
        if not line:
            continue
        for name, pattern, _ in GRAMMARS:
            match = pattern.match(line[0])
            if match:
                hits[name].append(match.groupdict())
                break
    return hits


def board_from_job(name):
    match = JOB_NAME.match(name or "")
    return match.group("board") if match else None


def failed_step(job):
    for step in job.get("steps") or []:
        if step.get("conclusion") in ("failure", "cancelled", "timed_out"):
            return step.get("name")
    return None


def summarise_board(job, hits):
    """One board's verdict: what happened, and the one line that explains it."""
    conclusion = job.get("conclusion") or job.get("status") or "unknown"
    result = {
        "board": board_from_job(job["name"]),
        "conclusion": conclusion,
        "url": job.get("html_url"),
        "attempts": 1,
        "cause": None,
        "retry_causes": [],
        "headroom": [],
    }

    retries = hits.get("retry", [])
    if retries:
        result["attempts"] = max(int(r["attempt"]) for r in retries) + 1
        # dict.fromkeys keeps first-seen order; a board that failed the same way
        # six times should say so once.
        result["retry_causes"] = list(dict.fromkeys(r["cause"] for r in retries))

    result["headroom"] = [(h["image"], int(h["left"]), int(h["cap"]))
                          for h in hits.get("headroom", [])]

    if hits.get("size"):
        result["cause"] = f"image over its partition ({hits['size'][0]['detail']})"
        result["attempts"] = 1
    elif hits.get("exhausted"):
        exhausted = hits["exhausted"][0]
        result["attempts"] = int(exhausted["attempts"])
        result["cause"] = exhausted["cause"]
    elif conclusion != "success":
        # Nothing this workflow annotated. Either a later step failed, or the
        # job never got far enough to say anything -- a runner that died, or a
        # cancellation. Both are worth naming distinctly: the first is a real
        # break somewhere other than the build, the second is a re-run away.
        step = failed_step(job)
        result["cause"] = (f"step: {step}" if step
                           else f"runner infrastructure or cancelled ({conclusion})")
    return result


# --------------------------------------------------------------------------
# Collection
# --------------------------------------------------------------------------

def collect_run(api, run_id, flakes=True, notes=None):
    jobs = api.paginate(f"/repos/{api.repo}/actions/runs/{run_id}/jobs",
                        {"filter": "latest"}, "jobs")
    boards = [job for job in jobs if board_from_job(job["name"])]

    wanted = [job for job in boards
              if flakes or (job.get("conclusion") not in (None, "success"))]

    def annotations(job):
        try:
            payload = api.get(f"/repos/{api.repo}/check-runs/{job['id']}/annotations")
            return job["id"], [entry.get("message") for entry in payload]
        except ApiError as error:
            return job["id"], error

    detail = {}
    if wanted:
        with ThreadPoolExecutor(max_workers=8) as pool:
            for job_id, value in pool.map(annotations, wanted):
                detail[job_id] = value

    degraded = sum(1 for value in detail.values() if isinstance(value, ApiError))
    if degraded and notes is not None:
        notes.append(f"{degraded} board(s) reported no annotation detail "
                     f"(the check-run lookup failed); their counts are still exact.")

    results = []
    for job in boards:
        messages = detail.get(job["id"])
        hits = parse_annotations(messages if isinstance(messages, list) else [])
        results.append(summarise_board(job, hits))
    results.sort(key=lambda r: r["board"])
    return results


def collect_history(api, window, notes=None):
    """Per-board pass/fail over the last `window` completed scheduled runs."""
    try:
        runs = api.get(f"/repos/{api.repo}/actions/workflows/build.yml/runs",
                       {"event": "schedule", "status": "completed",
                        "per_page": window})["workflow_runs"]
    except ApiError as error:
        if notes is not None:
            notes.append(f"streak history unavailable ({error}).")
        return []

    def one(run):
        try:
            jobs = api.paginate(f"/repos/{api.repo}/actions/runs/{run['id']}/jobs",
                                {"filter": "latest"}, "jobs")
        except ApiError:
            return None
        return {
            "id": run["id"],
            "date": (run.get("created_at") or "")[:10],
            "boards": {board_from_job(job["name"]): job.get("conclusion")
                       for job in jobs if board_from_job(job["name"])},
        }

    with ThreadPoolExecutor(max_workers=4) as pool:
        history = [entry for entry in pool.map(one, runs) if entry]
    # Newest first, which is the order the strip is read in.
    history.sort(key=lambda entry: entry["id"], reverse=True)
    return history


def streaks(history):
    """(board, current failure streak, newest-first strip) for boards that failed."""
    boards = sorted({board for entry in history for board in entry["boards"]})
    rows = []
    for board in boards:
        strip = []
        for entry in history:
            conclusion = entry["boards"].get(board)
            strip.append(PASS if conclusion == "success"
                         else ABSENT if conclusion is None else FAIL)
        if FAIL not in strip:
            continue
        streak = 0
        for mark in strip:
            if mark != FAIL:
                break
            streak += 1
        rows.append((board, streak, "".join(strip)))
    # Longest active streak first, then most failures overall.
    rows.sort(key=lambda row: (-row[1], -row[2].count(FAIL), row[0]))
    return rows


# --------------------------------------------------------------------------
# Rendering
# --------------------------------------------------------------------------

def report_title(event, build_id, run_id):
    """What to head the report with.

    preflight computes build_id as nightly-<date>-<sha> for every event, so on
    a pull request it names a nightly release that does not exist. Only the
    events that actually publish one get to use it.
    """
    if event in ("schedule", "workflow_dispatch") and build_id:
        return build_id
    return f"Build {run_id}"


def _board_list(boards, cap=8):
    shown = ", ".join(f"`{b}`" for b in boards[:cap])
    return shown if len(boards) <= cap else f"{shown} +{len(boards) - cap} more"


def render(results, history, title, calls, notes):
    failed = [r for r in results if r["conclusion"] != "success"]
    passed = [r for r in results if r["conclusion"] == "success"]
    flaked = [r for r in passed if r["attempts"] > 1]
    total = len(results)

    out = [f"## {title}", ""]
    if not total:
        out += ["No board jobs in this run.", ""]
        return "\n".join(out)

    if failed:
        out += [f"**{len(failed)} of {total} boards failed.** "
                f"{len(passed)} built.", ""]
    else:
        out += [f"**All {total} boards built.**"
                + (f" {len(flaked)} needed a retry." if flaked else ""), ""]

    if failed:
        out += ["### Failures by cause", "",
                "| Cause | Boards | Which |", "|---|---:|---|"]
        groups = defaultdict(list)
        for result in failed:
            groups[result["cause"] or "unclassified"].append(result)
        for cause, group in sorted(groups.items(),
                                   key=lambda kv: (-len(kv[1]), kv[0])):
            attempts = max(result["attempts"] for result in group)
            suffix = f" (after {attempts} attempts)" if attempts > 1 else ""
            boards = [result["board"] for result in group]
            out.append(f"| `{cause}`{suffix} | {len(boards)} "
                       f"| {_board_list(boards)} |")
        out.append("")

    if flaked:
        out += ["### Flakes the retry loop absorbed", "",
                "These boards went green. They are here because they nearly did not, "
                "and because the same cause exhausting its budget is what turns the "
                "nightly red.", "",
                "| Board | Attempts | Cause |", "|---|---:|---|"]
        for result in sorted(flaked, key=lambda r: -r["attempts"]):
            causes = ", ".join(f"`{c}`" for c in result["retry_causes"]) or "—"
            out.append(f"| `{result['board']}` | {result['attempts']} | {causes} |")
        out.append("")

    headroom = [(r["board"], image, left, cap)
                for r in results for image, left, cap in r["headroom"]]
    if headroom:
        out += ["### Headroom warnings", "",
                "Fitting with almost nothing to spare looks identical to fitting "
                "comfortably, right up until it does not fit.", "",
                "| Board | Image | Free | Cap |", "|---|---|---:|---:|"]
        for board, image, left, cap in sorted(headroom, key=lambda row: row[2]):
            out.append(f"| `{board}` | {image} | {left}KB | {cap}KB |")
        out.append("")

    rows = streaks(history)
    if history:
        out += [f"### Last {len(history)} scheduled runs", ""]
        if rows:
            out += ["Boards that failed at least once in the window, "
                    "newest run on the left.", "",
                    "| Board | Streak | History |", "|---|---|---|"]
            for board, streak, strip in rows:
                note = (f"**{streak} in a row**" if streak > 1
                        else "failed last night" if streak == 1
                        else f"{strip.count(FAIL)} in {len(strip)}")
                out.append(f"| `{board}` | {note} | `{strip}` |")
        else:
            out.append("Every board passed in every run in the window.")
        out.append("")

    for note in notes:
        out.append(f"> {note}")
    if notes:
        out.append("")
    out.append(f"<sub>{calls} API request(s).</sub>")
    return "\n".join(out) + "\n"


# --------------------------------------------------------------------------
# Self-test
# --------------------------------------------------------------------------

def _fixture_run():
    """The 2026-08-12 shape: one cause, most of the matrix, plus a flake."""
    jobs = []
    for index in range(94):
        jobs.append({
            "id": 1000 + index, "name": f"Firmware (board{index:02d}_lite)",
            "conclusion": "failure", "html_url": "u",
            "steps": [{"name": "Build firmware", "conclusion": "failure"}],
        })
    jobs += [
        {"id": 2001, "name": "Firmware (green01_lite)", "conclusion": "success",
         "html_url": "u", "steps": []},
        {"id": 2002, "name": "Firmware (flaky01_lite)", "conclusion": "success",
         "html_url": "u", "steps": []},
        {"id": 2003, "name": "Firmware (fat01_lite)", "conclusion": "failure",
         "html_url": "u", "steps": [{"name": "Build firmware",
                                     "conclusion": "failure"}]},
        {"id": 2004, "name": "Firmware (late01_lite)", "conclusion": "failure",
         "html_url": "u", "steps": [{"name": "Build firmware",
                                     "conclusion": "success"},
                                    {"name": "Verify kernel modules",
                                     "conclusion": "failure"}]},
        {"id": 2005, "name": "Firmware (dead01_lite)", "conclusion": "cancelled",
         "html_url": "u", "steps": []},
        {"id": 9999, "name": "Publish releases", "conclusion": "success",
         "html_url": "u", "steps": []},
    ]
    messages = {job["id"]: [] for job in jobs}
    for index in range(94):
        messages[1000 + index] = [
            "Node.js 20 is deprecated. The following actions target Node.js 20",
            f"board{index:02d}_lite: attempt 1 failed (majestic-webui-dist/target_installed), "
            "retrying after 30s",
            f"board{index:02d}_lite: build failed after 7 attempts — "
            "majestic-webui-dist/target_installed",
        ]
    messages[2001] = ["green01_lite: uImage has 25KB left of 2048KB"]
    messages[2002] = [
        "flaky01_lite: attempt 1 failed (opus-1.4/downloaded), retrying after 30s",
        "flaky01_lite: attempt 2 failed (opus-1.4/downloaded), retrying after 60s",
    ]
    messages[2003] = ["fat01_lite: -- size exceeded by: 4KB — the image does not fit "
                      "its partition, which retrying cannot change"]
    return jobs, messages


def self_test():
    problems = []

    jobs, messages = _fixture_run()
    results = []
    for job in jobs:
        if not board_from_job(job["name"]):
            continue
        results.append(summarise_board(job, parse_annotations(messages[job["id"]])))

    by_board = {r["board"]: r for r in results}

    # 1. Every board job, and nothing else, becomes a row.
    if len(results) != 99:
        problems.append(f"expected 99 board rows, got {len(results)}")
    if "Publish releases" in by_board:
        problems.append("a non-matrix job was counted as a board")

    # 2. The dominant cause groups as one cause, with its real attempt count.
    fail = by_board["board00_lite"]
    if fail["cause"] != "majestic-webui-dist/target_installed":
        problems.append(f"exhausted cause misparsed: {fail['cause']}")
    if fail["attempts"] != 7:
        problems.append(f"exhausted attempts misparsed: {fail['attempts']}")

    # 3. The Node.js deprecation warning every job carries is not a cause.
    if any("Node.js" in (r["cause"] or "") for r in results):
        problems.append("annotation noise leaked into a cause")

    # 4. A board that passed after retries is a flake, not a failure.
    flake = by_board["flaky01_lite"]
    if flake["conclusion"] != "success" or flake["attempts"] != 3:
        problems.append(f"absorbed flake misread: {flake}")
    if flake["retry_causes"] != ["opus-1.4/downloaded"]:
        problems.append(f"repeated retry cause not deduplicated: {flake['retry_causes']}")

    # 5. A deterministic size failure never claims to have been retried.
    fat = by_board["fat01_lite"]
    if "over its partition" not in (fat["cause"] or "") or fat["attempts"] != 1:
        problems.append(f"size failure misread: {fat}")

    # 6. A failure outside the build step is named by its step, and a job that
    #    never annotated anything is called infrastructure rather than a build
    #    break -- the two need different responses.
    if by_board["late01_lite"]["cause"] != "step: Verify kernel modules":
        problems.append(f"post-build failure misread: {by_board['late01_lite']}")
    if "infrastructure" not in (by_board["dead01_lite"]["cause"] or ""):
        problems.append(f"cancelled job misread: {by_board['dead01_lite']}")

    # 7. Headroom is reported for boards that passed.
    if by_board["green01_lite"]["headroom"] != [("uImage", 25, 2048)]:
        problems.append(f"headroom misparsed: {by_board['green01_lite']['headroom']}")

    # 8. Streaks: leading failures count, older ones do not, and a board that
    #    never failed stays out of the table entirely.
    history = [
        {"id": 5, "date": "d5", "boards": {"a": "failure", "b": "success", "c": "success"}},
        {"id": 4, "date": "d4", "boards": {"a": "failure", "b": "success", "c": "success"}},
        {"id": 3, "date": "d3", "boards": {"a": "success", "b": "failure", "c": "success"}},
        {"id": 2, "date": "d2", "boards": {"a": "success", "b": "success"}},
    ]
    rows = {board: (streak, strip) for board, streak, strip in streaks(history)}
    if "c" in rows:
        problems.append("a board that never failed was listed in the streak table")
    if rows.get("a") != (2, f"{FAIL}{FAIL}{PASS}{PASS}"):
        problems.append(f"leading streak wrong: {rows.get('a')}")
    if rows.get("b") != (0, f"{PASS}{PASS}{FAIL}{PASS}"):
        problems.append(f"non-leading failure counted as a streak: {rows.get('b')}")
    if streaks(history)[0][0] != "a":
        problems.append("streak table not ordered by active streak")
    # A board absent from an older run must not be read as a pass: the strip
    # has to be able to say "we do not know" or a newly added board reads as
    # having a clean fortnight behind it.
    absent = streaks([{"id": 2, "date": "d", "boards": {"a": "failure"}},
                      {"id": 1, "date": "d", "boards": {}}])
    if absent[0][2] != f"{FAIL}{ABSENT}":
        problems.append(f"missing board not marked absent: {absent}")

    # 9. The report says the two things a reader is actually after.
    text = render(results, history, "t", 120, [])
    for expected in ["97 of 99 boards failed", "majestic-webui-dist/target_installed",
                     "Flakes the retry loop absorbed", "Headroom warnings",
                     "120 API request(s)"]:
        if expected not in text:
            problems.append(f"summary is missing {expected!r}")
    green = render([r for r in results if r["conclusion"] == "success"], [], "t", 3, [])
    if "All 2 boards built." not in green:
        problems.append("an all-green matrix does not say so")

    # 10. A pull request's summary must not claim to be a nightly release.
    if report_title("schedule", "nightly-20260812-1fa881", "7") != "nightly-20260812-1fa881":
        problems.append("a scheduled run lost its build id")
    for event in ("pull_request", ""):
        if report_title(event, "nightly-20260812-1fa881", "7") != "Build 7":
            problems.append(f"a {event or 'bare'} run was titled as a nightly")
    if report_title("schedule", "", "7") != "Build 7":
        problems.append("a missing build id did not fall back to the run id")

    # 11. THE DRIFT GATE. Every grammar's anchor must still be a literal in
    #     build.yml, and build.yml must not have grown an annotation in the
    #     matrix job that nothing here accounts for. A reworded ::error:: is
    #     otherwise indistinguishable, from this side, from a quiet night.
    with open(WORKFLOW, encoding="utf-8") as handle:
        workflow = handle.read()
    for name, _, anchor in GRAMMARS:
        if anchor not in workflow:
            problems.append(f"grammar {name!r} no longer matches build.yml: "
                            f"{anchor!r} is not in the workflow")
    # Slice out the matrix job by name rather than by position: jobs get
    # added between them (this file's own `summary` job did), and a split that
    # depended on the order would start reading the wrong block and pass.
    blocks = re.split(r"^  ([A-Za-z][\w-]*):$", workflow, flags=re.M)
    jobs = dict(zip(blocks[1::2], blocks[2::2]))
    if "buildroot" not in jobs:
        problems.append("build.yml has no `buildroot` job to check annotations in")
    emitted = re.findall(r"::(?:error|warning)::(.*)", jobs.get("buildroot", ""))
    accounted = [anchor for _, _, anchor in GRAMMARS] + IGNORED_ANCHORS
    for line in emitted:
        if not any(anchor.split("${")[0] in line or anchor in line
                   for anchor in accounted):
            problems.append(f"build.yml emits an annotation nothing here reads: {line!r}")
    if len(emitted) != len(GRAMMARS) + len(IGNORED_ANCHORS):
        problems.append(f"build.yml's matrix job emits {len(emitted)} annotations, "
                        f"{len(GRAMMARS) + len(IGNORED_ANCHORS)} are classified")

    for problem in problems:
        print(f"build-summary: {problem}", file=sys.stderr)
    if problems:
        return 1
    print(f"build-summary: self-test ok ({len(GRAMMARS)} grammars, "
          f"{len(emitted)} annotations in build.yml, {len(results)} fixture boards)")
    return 0


# --------------------------------------------------------------------------
# Entry point
# --------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--run-id")
    parser.add_argument("--repo", default=os.environ.get("GITHUB_REPOSITORY"))
    parser.add_argument("--event", default=os.environ.get("GITHUB_EVENT_NAME", ""))
    parser.add_argument("--build-id", default="")
    parser.add_argument("--history", type=int, default=None,
                        help="scheduled runs to look back over; 0 disables")
    parser.add_argument("--no-flakes", dest="flakes", action="store_false",
                        help="skip the annotation fetch for boards that passed")
    parser.add_argument("--out", default=os.environ.get("GITHUB_STEP_SUMMARY"),
                        help="'-' for stdout")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    token = os.environ.get("GH_TOKEN") or os.environ.get("GITHUB_TOKEN")
    if not (token and args.repo and args.run_id):
        print("build-summary: need GH_TOKEN, --repo/GITHUB_REPOSITORY and --run-id",
              file=sys.stderr)
        return 2

    # Streaks are only meaningful over runs that built the same set of boards.
    # A pull_request run is narrowed by the selector, so it gets the per-run
    # half of the report and nothing else.
    window = args.history
    if window is None:
        window = HISTORY_RUNS if args.event in ("schedule", "workflow_dispatch") else 0

    api = Api(token, args.repo)
    notes = []
    results = collect_run(api, args.run_id, flakes=args.flakes, notes=notes)
    history = collect_history(api, window, notes=notes) if window else []

    title = report_title(args.event, args.build_id, args.run_id)
    report = render(results, history, title, api.calls, notes)

    failed = sum(1 for r in results if r["conclusion"] != "success")
    print(f"build-summary: {failed} of {len(results)} boards failed, "
          f"{api.calls} API request(s)")

    if args.out in (None, "-"):
        sys.stdout.write(report)
    else:
        with open(args.out, "a", encoding="utf-8") as handle:
            handle.write(report)
    return 0


if __name__ == "__main__":
    sys.exit(main())
