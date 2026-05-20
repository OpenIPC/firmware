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
from pathlib import Path

REPO = os.environ.get("GITHUB_REPOSITORY", "OpenIPC/firmware")
RETENTION = 90
TAG_RE = re.compile(r"^nightly-(\d{8})-([0-9a-f]{7})$")
ASSET_RE = re.compile(r"^openipc\.([^.]+)-(nor|nand)-(lite|ultimate|neo)\.tgz$")


def gh(*args: str) -> str:
    return subprocess.check_output(["gh", *args], text=True)


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


def main() -> None:
    out_dir = Path(sys.argv[1] if len(sys.argv) > 1 else ".")
    out_dir.mkdir(parents=True, exist_ok=True)

    now = dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds").replace("+00:00", "Z")
    dated = list_dated_releases()

    if not dated:
        manifest = {"schema": 1, "generated_at": now,
                    "channels": {}, "builds": []}
        (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        (out_dir / "manifest.flat").write_text(
            f"# generated_at={now}\n"
            "# No nightly-YYYYMMDD-<short> releases yet — "
            "the first scheduled build will populate this index.\n"
        )
        print("manifest: 0 builds (empty index)")
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
    (out_dir / "manifest.flat").write_text("\n".join(lines) + "\n")

    print(f"manifest: {len(builds)} builds, newest={newest}")


if __name__ == "__main__":
    main()
