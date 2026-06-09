#!/usr/bin/env python3
"""Build manifest.json and manifest.flat for the nightly-* release index.

Reads release metadata via the `gh` CLI (auth via $GH_TOKEN) and writes
two files into the directory given as the first argument:

  - manifest.json — full index for hosts, agents, CI tools
  - manifest.flat — whitespace-delimited index for busybox-shell consumers
                    (on-device sysupgrade)

md5 is intentionally omitted from the v1 schema. Each `.tgz` already
ships an in-archive `.md5sum` sidecar that sysupgrade validates after
download (`general/overlay/usr/sbin/sysupgrade:93-95`), so a manifest-
level md5 would be redundant and expensive to compute (would require
downloading every asset on each manifest rebuild).
"""
from __future__ import annotations

import datetime as dt
import json
import os
import re
import subprocess
import sys
import time
from pathlib import Path

REPO = os.environ.get("GITHUB_REPOSITORY", "OpenIPC/firmware")
RETENTION = 90
TAG_RE = re.compile(r"^nightly-(\d{8})-([0-9a-f]{7})$")
ASSET_RE = re.compile(r"^openipc\.([^.]+)-(nor|nand)-(lite|ultimate|neo)\.tgz$")

# Defconfig lines for the SoC-alias scan: a published image's SOC_MODEL plus
# the space-separated retired/compatible ids it also serves (SOC_ALIASES).
SOC_MODEL_RE = re.compile(r'^BR2_OPENIPC_SOC_MODEL\s*=\s*"?([A-Za-z0-9]+)"?\s*$')
SOC_ALIASES_RE = re.compile(r'^BR2_OPENIPC_SOC_ALIASES\s*=\s*"?([^"\n]*)"?\s*$')

# Retry budget for transient GitHub API failures (HTTP 401 Bad credentials,
# 5xx, rate-limit) observed on workflow_run-triggered runs 2026-05-23.
GH_RETRY_DELAYS = (0, 5, 15, 40)  # 4 attempts; last delay before final try


def gh(*args: str) -> str:
    # Always pass --repo so we don't depend on a .git in cwd
    # (the workflow runs the script from a path without .git).
    # Retry on transient failures (the GitHub API/token plane has flaky days);
    # surface to stderr so failures are visible in the action log.
    cmd = ["gh", *args, "--repo", REPO]
    last_exc = None
    for delay in GH_RETRY_DELAYS:
        if delay > 0:
            time.sleep(delay)
        try:
            return subprocess.check_output(cmd, text=True, stderr=subprocess.PIPE)
        except subprocess.CalledProcessError as e:
            last_exc = e
            err = e.stderr or ""
            # Permanent failures — don't waste the retry budget.
            if "release not found" in err.lower() or "not found (HTTP 404)" in err:
                break
            sys.stderr.write(
                f"gh {' '.join(args[:3])}: attempt failed "
                f"(rc={e.returncode}): {err.strip()[:240]}\n"
            )
    sys.stderr.write(
        f"gh {' '.join(args[:3])}: giving up; "
        f"final stderr:\n{last_exc.stderr}\n"
    )
    raise last_exc


def list_dated_releases() -> list[dict]:
    raw = gh("release", "list", "--limit", "200",
             "--json", "tagName,createdAt,isPrerelease")
    rels = json.loads(raw)
    dated = [r for r in rels if TAG_RE.match(r["tagName"])]
    dated.sort(key=lambda r: r["createdAt"], reverse=True)
    return dated[:RETENTION]


def fetch_release(tag: str) -> dict:
    raw = gh("release", "view", tag,
             "--json", "tagName,createdAt,body,assets")
    return json.loads(raw)


def parse_body(body: str | None) -> tuple[str, str, str]:
    sha = short = built_at = ""
    for line in (body or "").splitlines():
        if line.startswith("sha="):
            sha = line[4:].strip()
        elif line.startswith("short="):
            short = line[6:].strip()
        elif line.startswith("built_at="):
            built_at = line[9:].strip()
    return sha, short, built_at


def parse_asset(name: str) -> tuple[str, str] | None:
    m = ASSET_RE.match(name)
    if not m:
        return None
    soc, flash, variant = m.groups()
    return f"{soc}_{variant}", flash


def scan_aliases() -> dict[str, str]:
    """Map each retired/compatible SoC id -> the canonical SOC_MODEL it is
    published under, read from BR2_OPENIPC_SOC_ALIASES in the in-tree
    defconfigs. Lets on-device sysupgrade route a camera still reporting the
    old id (xm550, gk7205v210, hi3516cv610, ...) to the image that exists.
    Best-effort: returns {} if the defconfig tree is not beside this script.
    """
    try:
        root = Path(__file__).resolve().parents[2]
    except (IndexError, OSError):
        return {}
    aliases: dict[str, str] = {}
    for cfg in sorted(root.glob("br-ext-chip-*/configs/*_defconfig")):
        try:
            text = cfg.read_text()
        except OSError:
            continue
        model = ""
        alias_field = ""
        for line in text.splitlines():
            m = SOC_MODEL_RE.match(line)
            if m:
                model = m.group(1)
                continue
            a = SOC_ALIASES_RE.match(line)
            if a:
                alias_field = a.group(1)
        if not model or not alias_field.strip():
            continue
        for chip in alias_field.split():
            if not chip or chip == model:
                continue
            prev = aliases.get(chip)
            if prev and prev != model:
                sys.stderr.write(
                    f"alias conflict: {chip} -> {prev} and {model}; keeping {prev}\n"
                )
                continue
            aliases[chip] = model
    return dict(sorted(aliases.items()))


def main() -> None:
    out_dir = Path(sys.argv[1] if len(sys.argv) > 1 else ".")
    out_dir.mkdir(parents=True, exist_ok=True)

    aliases = scan_aliases()
    now = dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds").replace("+00:00", "Z")
    dated = list_dated_releases()

    if not dated:
        manifest = {"schema": 1, "generated_at": now,
                    "channels": {}, "aliases": aliases, "builds": []}
        (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        flat = [
            f"# generated_at={now}",
            "# No nightly-YYYYMMDD-<short> releases yet — "
            "the first scheduled build will populate this index.",
        ]
        for chip, model in aliases.items():
            flat.append(f"@alias {chip} {model}")
        (out_dir / "manifest.flat").write_text("\n".join(flat) + "\n")
        print(f"manifest: 0 builds (empty index), {len(aliases)} aliases")
        return

    builds = []
    for rel in dated:
        info = fetch_release(rel["tagName"])
        sha, short, built_at = parse_body(info.get("body") or "")
        platforms: dict[str, dict[str, dict]] = {}
        for a in info.get("assets") or []:
            parsed = parse_asset(a["name"])
            if not parsed:
                continue
            platform, flash = parsed
            platforms.setdefault(platform, {})[flash] = {
                "url": a["url"],
                "size": a["size"],
            }
        builds.append({
            "id": info["tagName"],
            "sha": sha,
            "short": short,
            "built_at": built_at or info["createdAt"],
            "release_url": f"https://github.com/{REPO}/releases/tag/{info['tagName']}",
            "platforms": platforms,
        })

    newest = builds[0]["id"]
    manifest = {
        "schema": 1,
        "generated_at": now,
        "channels": {"nightly": newest, "latest": newest},
        "aliases": aliases,
        "builds": builds,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")

    lines = [
        "# OpenIPC firmware build index",
        f"# generated_at={now}",
        "# columns: build_id platform flash size url",
    ]
    for b in builds:
        for platform, flashes in sorted(b["platforms"].items()):
            for flash, info in sorted(flashes.items()):
                lines.append(f"{b['id']} {platform} {flash} {info['size']} {info['url']}")
    lines.append("# channels")
    for ch, target in manifest["channels"].items():
        lines.append(f"@channel {ch} {target}")
    if aliases:
        lines.append("# aliases")
        for chip, model in aliases.items():
            lines.append(f"@alias {chip} {model}")
    (out_dir / "manifest.flat").write_text("\n".join(lines) + "\n")

    print(f"manifest: {len(builds)} builds, newest={newest}")


if __name__ == "__main__":
    main()
