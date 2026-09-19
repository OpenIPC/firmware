#!/usr/bin/env python3
"""Check .github/ISSUE_TEMPLATE/ against GitHub's issue-form schema.

Issue forms are read from the default branch and nowhere else, so a form with a
bad element type or a misplaced `accept:` cannot be caught by opening a pull
request -- GitHub silently drops the broken template from the chooser after the
merge, and the first sign of trouble is a reporter who cannot file. This is the
only pre-merge check there is.

It also holds two things together that drift apart silently: the SoC-vendor
dropdown the forms ask for, and the vendor list the labeller in
.github/workflows/issue-labeler.yml maps to platform:<Vendor>. Add a vendor to
one and not the other and every issue from that silicon lands on
platform:Unknown, which is the exact failure the labeller was written to end.

  python3 .github/scripts/lint-issue-forms.py              # lint the tree
  python3 .github/scripts/lint-issue-forms.py --self-test  # ...and the labeller's own rules
  python3 .github/scripts/lint-issue-forms.py --remote <owner/repo>   # ask GitHub itself

GitHub's own verdict on a template is readable without a browser session, which
matters because the template chooser is not: it redirects to a login. A file's
**blob page** is public and carries the parsed result inline, errors and all.

  https://github.com/<owner>/<repo>/blob/HEAD/.github/ISSUE_TEMPLATE/<file>

--remote fetches those and prints what GitHub says. The files have to be on the
repository's DEFAULT BRANCH for a verdict to exist, so point it at a fork you
have pushed to, never at a pull request branch.

Stdlib plus PyYAML, which `make deps` already installs.
"""

import json
import os
import re
import shutil
import subprocess
import sys

import yaml

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
TEMPLATE_DIR = os.path.join(REPO_ROOT, ".github", "ISSUE_TEMPLATE")
LABELER = os.path.join(REPO_ROOT, ".github", "workflows", "issue-labeler.yml")

TOP_LEVEL = {"name", "description", "body", "title", "labels", "assignees", "projects", "type"}
REQUIRED_TOP = {"name", "description", "body"}

# attribute name -> which element types accept it.
ATTRS = {
    "markdown":   {"value"},
    "input":      {"label", "description", "placeholder", "value"},
    "textarea":   {"label", "description", "placeholder", "value", "render"},
    "dropdown":   {"label", "description", "multiple", "options", "default"},
    "checkboxes": {"label", "description", "options"},
    "upload":     {"label", "description"},
}
REQUIRED_ATTRS = {
    "markdown": {"value"},
    "input": {"label"},
    "textarea": {"label"},
    "dropdown": {"label", "options"},
    "checkboxes": {"label", "options"},
    "upload": {"label"},
}
# markdown is decorative and takes no id and no validations.
NO_ID = {"markdown"}
VALIDATIONS = {
    "input": {"required"},
    "textarea": {"required"},
    "dropdown": {"required"},
    "checkboxes": {"required"},
    "upload": {"required", "accept"},
}

# What an `accept:` list may name, by category, with its size cap.
#
# `.tar.gz` is the trap, and it cost a round of bisection to find. GitHub's own
# documentation lists .tar.gz among the archive types the uploader takes, and it
# does take them -- but an accept list matches a single extension, so naming
# .tar.gz there fails validation with "contains invalid file extensions:
# .tar.gz" and the entire template is dropped from the chooser with nothing
# said. A .tar.gz file still uploads under `.gz`, so nothing is lost by leaving
# it out. All six forms carried it on the first attempt and not one of them
# rendered.
ACCEPTED = {
    ".zip": 25, ".gz": 25,
    ".pdf": 25, ".docx": 25, ".xlsx": 25, ".pptx": 25,
    ".png": 10, ".jpg": 10, ".jpeg": 10, ".gif": 10, ".svg": 10, ".webp": 10,
    ".mp4": 100, ".mov": 100, ".webm": 100,
    ".json": 25, ".py": 25, ".js": 25, ".ts": 25, ".log": 25, ".txt": 25, ".csv": 25,
}

# The labeller finds the dropdown with /^###\s+SoC vendor.*$/mi, so any label
# starting with these words is one it will read -- and therefore one this check
# has to cover. The developer form's "SoC vendor, if it is specific to one" is
# exactly that case, and an exact-string match here would have skipped it.
VENDOR_DROPDOWN = "soc vendor"
# The developer form lets a report say it is not about one SoC; the labeller
# treats that answer as "no label", so it is legal in the dropdown and is not a
# vendor the labeller has to know.
NON_VENDOR_OPTIONS = {"I don't know", "Not SoC specific"}


def fail(errors, path, message):
    errors.append(f"{os.path.relpath(path, REPO_ROOT)}: {message}")


def lint_config(path, errors):
    with open(path) as handle:
        doc = yaml.safe_load(handle)
    if not isinstance(doc, dict):
        fail(errors, path, "config.yml must be a mapping")
        return
    unknown = set(doc) - {"blank_issues_enabled", "contact_links"}
    if unknown:
        fail(errors, path, f"unknown top-level key(s): {sorted(unknown)}")
    for i, link in enumerate(doc.get("contact_links") or []):
        missing = {"name", "url", "about"} - set(link)
        if missing:
            fail(errors, path, f"contact_links[{i}] missing {sorted(missing)}")
        if not str(link.get("url", "")).startswith("https://"):
            fail(errors, path, f"contact_links[{i}] url is not https")


def lint_form(path, errors, names, vendor_sets):
    with open(path) as handle:
        doc = yaml.safe_load(handle)
    if not isinstance(doc, dict):
        fail(errors, path, "a form must be a mapping")
        return

    unknown = set(doc) - TOP_LEVEL
    if unknown:
        fail(errors, path, f"unknown top-level key(s): {sorted(unknown)}")
    missing = REQUIRED_TOP - set(doc)
    if missing:
        fail(errors, path, f"missing required top-level key(s): {sorted(missing)}")
        return

    # The chooser keys on `name`, so a duplicate silently hides one form.
    if doc["name"] in names:
        fail(errors, path, f"name {doc['name']!r} is already used by {names[doc['name']]}")
    names[doc["name"]] = os.path.basename(path)

    seen_ids = set()
    for i, element in enumerate(doc["body"]):
        where = f"body[{i}]"
        kind = element.get("type")
        if kind not in ATTRS:
            fail(errors, path, f"{where}: unknown element type {kind!r}")
            continue

        if "id" in element:
            if kind in NO_ID:
                fail(errors, path, f"{where}: a {kind} element takes no id")
            elif element["id"] in seen_ids:
                fail(errors, path, f"{where}: duplicate id {element['id']!r}")
            else:
                seen_ids.add(element["id"])

        attrs = element.get("attributes") or {}
        bad = set(attrs) - ATTRS[kind]
        if bad:
            fail(errors, path, f"{where}: {kind} has no attribute(s) {sorted(bad)}")
        lacking = REQUIRED_ATTRS[kind] - set(attrs)
        if lacking:
            fail(errors, path, f"{where}: {kind} needs {sorted(lacking)}")

        validations = element.get("validations") or {}
        if validations and kind not in VALIDATIONS:
            fail(errors, path, f"{where}: a {kind} element takes no validations")
        bad = set(validations) - VALIDATIONS.get(kind, set())
        if bad:
            fail(errors, path, f"{where}: {kind} validations has no key(s) {sorted(bad)}")

        if kind == "upload":
            for ext in [e.strip() for e in str(validations.get("accept", "")).split(",") if e.strip()]:
                if ext not in ACCEPTED:
                    fail(errors, path,
                         f"{where}: accept lists {ext!r}, which GitHub's uploader refuses")

        if kind in ("dropdown", "checkboxes"):
            options = attrs.get("options") or []
            if not options:
                fail(errors, path, f"{where}: options cannot be empty")
            # A bare Yes / No / On / Off / True / False in the option list is
            # a YAML 1.1 boolean, so it reaches GitHub as `false` rather than
            # "No", the whole form fails schema validation, and it is dropped
            # from the chooser with nothing said. str() would hide it here, so
            # the type is checked before anything else touches these.
            for j, option in enumerate(options):
                if kind == "dropdown" and not isinstance(option, str):
                    fail(errors, path,
                         f"{where}: option {j} is {option!r} ({type(option).__name__}), "
                         "not a string -- quote it")
                if kind == "checkboxes":
                    if not isinstance(option, dict) or "label" not in option:
                        fail(errors, path, f"{where}: option {j} needs a label")
                    elif not isinstance(option["label"], str):
                        fail(errors, path,
                             f"{where}: option {j} label is {option['label']!r}, not a string")

            if kind == "dropdown":
                labels = [str(o) for o in options]
                if len(set(labels)) != len(labels):
                    fail(errors, path, f"{where}: dropdown options must be distinct")
                if str(attrs.get("label", "")).lower().startswith(VENDOR_DROPDOWN):
                    vendor_sets[os.path.basename(path)] = [
                        o for o in labels if o not in NON_VENDOR_OPTIONS
                    ]

        # A form that asks for a photo but forgets `accept` gets the reporter a
        # rejected .bmp and no explanation.
        if kind == "upload" and "accept" not in validations:
            fail(errors, path, f"{where}: an upload element should pin `accept`")


def labeller_vendors(errors):
    """The vendor list the workflow's script actually carries."""
    with open(LABELER) as handle:
        source = handle.read()
    match = re.search(r"const VENDORS = \[(.*?)\];", source, re.S)
    if not match:
        errors.append(".github/workflows/issue-labeler.yml: no VENDORS array found")
        return None
    return re.findall(r"'([^']+)'", match.group(1))


def run_labeller(bodies):
    """Run the workflow's own vendorLabel() over synthetic issue bodies.

    The point is to exercise the shipped JavaScript rather than a Python
    restatement of it -- a copy would keep passing after the real one broke. The
    bodies are hand-built strings, never a real issue: a labeller is tested by
    feeding it input, not by opening issues on a public tracker to see what
    sticks.
    """
    with open(LABELER) as handle:
        source = handle.read()
    match = re.search(r"(const VENDORS = \[.*?\n            \})\n\n", source, re.S)
    if not match:
        raise SystemExit("could not extract vendorLabel() from the workflow")
    fn = re.sub(r"^            ", "", match.group(1), flags=re.M)
    script = fn + "\nconst out = JSON.parse(process.argv[1]).map(vendorLabel);\n" \
                  "console.log(JSON.stringify(out));\n"

    if shutil.which("node"):
        cmd = ["node", "-e", script, json.dumps(bodies)]
    elif shutil.which("docker"):
        # No node on this machine; run the real thing in a container rather
        # than skipping the only test that touches the shipped code.
        cmd = ["docker", "run", "--rm", "-i", "node:22-alpine",
               "node", "-e", script, json.dumps(bodies)]
    else:
        raise SystemExit("self-test needs node or docker; neither is on PATH")

    done = subprocess.run(cmd, capture_output=True, text=True)
    if done.returncode != 0:
        raise SystemExit(f"labeller script failed:\n{done.stderr}")
    return json.loads(done.stdout)


def self_test():
    body = "### SoC vendor\n\n{}\n\n### SoC model\n\nx\n"
    cases = [
        (body.format("HiSilicon"), "platform:HiSilicon"),
        (body.format("SigmaStar"), "platform:SigmaStar"),
        (body.format("GrainMedia"), "platform:GrainMedia"),
        (body.format("TI"), "platform:TI"),
        # Case and stray whitespace are the reporter's, not ours to punish.
        (body.format("  goke  "), "platform:Goke"),
        # The explicit "I don't know" is the whole reason platform:Unknown
        # exists -- it must land there and not be dropped.
        (body.format("I don't know"), "platform:Unknown"),
        # A vendor nobody has heard of is unknown, not a crash.
        (body.format("Acme Semiconductors"), "platform:Unknown"),
        # GitHub's own spelling for a skipped optional field.
        (body.format("_No response_"), None),
        # The developer form's opt-out.
        (body.format("Not SoC specific"), None),
        # The free-form developer report, and an issue written before the forms
        # existed: no heading, so no label.
        ("Just some prose about a bug.\n", None),
        ("", None),
        # A heading that merely mentions the words must not match.
        ("### Notes about the SoC vendor situation\n\nHiSilicon\n", None),
        # Qodo caught this on #2454. 6-dev.yml renders its free-form Report
        # field BEFORE the dropdown, so a reporter who types the field's own
        # heading into their prose -- which anyone filing a bug about these
        # forms would do -- used to have that read as the answer. Two headings
        # means label nothing; a wrong platform label is worse than none.
        ("### Report\n\nprose\n\n### SoC vendor\n\nAmbarella\n\n"
         "### SoC vendor, if it is specific to one\n\nNot SoC specific\n", None),
        # ...and the same duplicate after the real field.
        ("### SoC vendor\n\nHiSilicon\n\n### What happens\n\n"
         "### SoC vendor\n\nGoke\n", None),
        # An inline mention is not a heading, so the one real field still wins.
        # Worth pinning: the fix must not become "give up whenever the words
        # appear", which would label nothing on half the sensor reports.
        ("### SoC vendor\n\nHiSilicon\n\n### What happens\n\n"
         "I quoted the form's ### SoC vendor line at it.\n", "platform:HiSilicon"),
        # A fenced log that happens to contain the heading is the same problem:
        # `render:` wraps a textarea in a code fence, it does not escape it.
        ("### SoC vendor\n\nGoke\n\n### Evidence\n\n```shell\n"
         "### SoC vendor\n\nTI\n```\n", None),
    ]
    got = run_labeller([b for b, _ in cases])
    bad = 0
    for (b, want), have in zip(cases, got):
        if have != want:
            bad += 1
            print(f"FAIL {b[:48]!r}\n  want {want!r}\n  got  {have!r}")
    if bad:
        raise SystemExit(f"{bad} labeller case(s) failed")
    print(f"labeller: {len(cases)} cases pass")


def remote(repo):
    """Print GitHub's own validation verdict for each template on a repo.

    The blob page embeds the parse result as JSON. A valid form carries
    "errors":[]; an invalid one names the element and the reason -- which is the
    only place that reason is ever stated, since the chooser simply omits the
    form and the repository looks fine.
    """
    import urllib.request

    api = f"https://api.github.com/repos/{repo}/contents/.github/ISSUE_TEMPLATE"
    with urllib.request.urlopen(api) as handle:
        names = [e["name"] for e in json.load(handle) if e["name"].endswith((".yml", ".yaml"))]

    bad = 0
    for name in sorted(names):
        url = f"https://github.com/{repo}/blob/HEAD/.github/ISSUE_TEMPLATE/{name}"
        req = urllib.request.Request(url, headers={"User-Agent": "lint-issue-forms"})
        with urllib.request.urlopen(req) as handle:
            page = handle.read().decode("utf-8", "replace")
        verdicts = re.findall(r'"errors":\[(.*?)\]', page)
        messages = re.findall(r'"message":"(.*?)"', " ".join(verdicts))
        if messages:
            bad += 1
            print(f"  REJECTED  {name}")
            for m in messages:
                print(f"              {m}")
        elif verdicts:
            print(f"  ok        {name}")
        elif name == "config.yml":
            print(f"  ok        {name}  (chooser config)")
        else:
            bad += 1
            print(f"  NO VERDICT {name} -- is it on the default branch?")
    if bad:
        raise SystemExit(f"{bad} template(s) GitHub will not show")
    print(f"GitHub accepts every template on {repo}")


def main():
    if "--remote" in sys.argv:
        remote(sys.argv[sys.argv.index("--remote") + 1])
        return

    errors = []
    names = {}
    vendor_sets = {}

    if not os.path.isdir(TEMPLATE_DIR):
        raise SystemExit(f"no {TEMPLATE_DIR}")

    for entry in sorted(os.listdir(TEMPLATE_DIR)):
        path = os.path.join(TEMPLATE_DIR, entry)
        if entry == "config.yml":
            lint_config(path, errors)
        elif entry.endswith((".yml", ".yaml")):
            lint_form(path, errors, names, vendor_sets)
        else:
            errors.append(f".github/ISSUE_TEMPLATE/{entry}: not a form or config.yml")

    known = labeller_vendors(errors)
    if known is not None:
        for form, vendors in sorted(vendor_sets.items()):
            unknown = [v for v in vendors if v not in known]
            if unknown:
                errors.append(
                    f".github/ISSUE_TEMPLATE/{form}: dropdown offers {unknown}, "
                    "which issue-labeler.yml cannot map to a platform: label")
        # The other direction matters less but still rots: a vendor the labeller
        # knows and no form offers is dead code.
        offered = {v for vendors in vendor_sets.values() for v in vendors}
        orphan = [v for v in known if v not in offered]
        if orphan:
            errors.append(
                f".github/workflows/issue-labeler.yml: knows {orphan}, "
                "which no form offers")

    if errors:
        for e in errors:
            print(f"error: {e}")
        raise SystemExit(f"{len(errors)} problem(s)")

    print(f"issue forms: {len(names)} forms, {len(vendor_sets)} vendor dropdowns, config.yml ok")
    if "--self-test" in sys.argv:
        self_test()


if __name__ == "__main__":
    main()
