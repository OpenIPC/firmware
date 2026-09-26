#!/usr/bin/env python3
"""Tell openipc.org about a published build, once.

openipc.org learns about a build from this push, sent once when the build has
published. It never polls GitHub for it, and no metadata files are uploaded
to the release for it to find. The contract lives with the receiver:
https://github.com/OpenIPC/website/blob/master/service/internal/builds/PUSH.md

Authorization is the job's GitHub Actions OIDC token, requested with audience
https://openipc.org. There is no shared secret. The job needs
`permissions: id-token: write`.

  gh release view "$BUILD_ID" --json assets > release.json
  push_build.py --source firmware --build-id nightly-20260925-230295e \
      --sha <40 hex> --built-at 2026-09-25T17:48:37Z --reports reports \
      --release-json release.json --match '^openipc\..*\.tgz$' --aliases-root .

  push_build.py ... --out payload.json    assemble and write, do not send

Stdlib only: the report job installs nothing.
"""
from __future__ import annotations

import argparse
import datetime as dt
import glob
import gzip
import hashlib
import json
import os
import re
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

AUDIENCE = "https://openipc.org"
DEFAULT_URL = "https://openipc.org"
ATTEMPTS = 5
FIRST_DELAY = 5  # seconds, doubling

# openipc.<board>-<storage>-<edition>.tgz: the platform is <board>-<edition>,
# the same name size_report.py and kconfig_graph.py give their files.
FIRMWARE_ASSET_RE = re.compile(r"^openipc\.([^.]+?)-(nor|nand|emmc|sd)-([a-z0-9]+)\.tgz$")
SIDECAR_RE = re.compile(r"^(sizes|kconfig-graph|kconfig-help)\.(.+)\.json$")


def sha256_of(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def release_assets(path: str | None) -> dict[str, dict]:
    """name -> {size, digest} from the release as GitHub reports it.

    Accepts `gh release view <tag> --json assets` output or a REST release
    object; both carry `assets[].{name,size,digest}`. GitHub computes the
    digest ("sha256:<hex>") itself when an asset is uploaded, so the push
    describes exactly the bytes a download returns, and nothing has to be kept
    around to hash afterwards.
    """
    if not path:
        return {}
    doc = json.loads(Path(path).read_text())
    out = {}
    for a in doc.get("assets", []):
        digest = a.get("digest") or ""
        out[a["name"]] = {
            "size": int(a["size"]),
            "sha256": digest[len("sha256:"):] if digest.startswith("sha256:") else "",
        }
    return out


def collect_assets(published: dict[str, dict], match: str | None,
                   patterns: list[str]) -> list[dict]:
    """The assets to push, sorted by name.

    - Every release asset whose name matches `match`, with GitHub's size and
      digest. An asset GitHub has no digest for is an error: the push would
      otherwise describe a file nobody hashed.
    - Every local file matching `patterns`, with the release's size and digest
      when the release has them, and its own otherwise (uboot uploads to
      `latest` and hashes locally only as a fallback).
    """
    out: dict[str, dict] = {}
    if match:
        rx = re.compile(match)
        for name, a in published.items():
            if not rx.search(name):
                continue
            if not a["sha256"]:
                raise SystemExit(f"::error::GitHub reports no digest for {name}")
            out[name] = {"name": name, **a}
    for pattern in patterns:
        for p in glob.glob(pattern):
            path = Path(p)
            if not path.is_file() or path.name in out:
                continue
            a = published.get(path.name)
            if a and a["sha256"]:
                out[path.name] = {"name": path.name, **a}
            else:
                out[path.name] = {"name": path.name, "size": path.stat().st_size,
                                  "sha256": sha256_of(path)}
    return [out[n] for n in sorted(out)]


def collect_platforms(dist: Path | None, assets: list[dict]) -> list[dict]:
    """One entry per platform that built.

    A platform is named by its size report and kconfig files and, for
    firmware, by its tarballs. A board whose size report failed still has a
    tarball, so it is still listed, without `sizes`.
    """
    docs: dict[str, dict] = {}
    if dist is not None and dist.is_dir():
        for path in sorted(dist.iterdir()):
            m = SIDECAR_RE.match(path.name)
            if not m or not path.is_file():
                continue
            kind, plat = m.groups()
            key = {"sizes": "sizes", "kconfig-graph": "kconfig_graph",
                   "kconfig-help": "kconfig_help"}[kind]
            try:
                docs.setdefault(plat, {})[key] = json.loads(path.read_text())
            except (OSError, ValueError) as e:
                # A broken sidecar costs that platform its detail, never the push.
                print(f"::warning::{path.name} is unreadable ({e}); pushing {plat} without it")
    for a in assets:
        m = FIRMWARE_ASSET_RE.match(a["name"])
        if m:
            board, _storage, edition = m.groups()
            docs.setdefault(f"{board}-{edition}", {})
    return [{"name": plat, **docs[plat]} for plat in sorted(docs)]


def build_payload(args: argparse.Namespace, now: dt.datetime) -> dict:
    assets = collect_assets(release_assets(args.release_json), args.match, args.assets)
    if not assets:
        raise SystemExit("::error::no assets matched; nothing published, nothing to push")
    payload: dict = {
        "schema": 1,
        "source": args.source,
        "build": {
            "id": args.build_id,
            "release": args.release or args.build_id,
            "sha": args.sha,
            "built_at": args.built_at,
            "published_at": now.strftime("%Y-%m-%dT%H:%M:%SZ"),
        },
        "assets": assets,
    }
    if args.webui_digest:
        payload["build"]["webui_digest"] = args.webui_digest
    if args.aliases_root:
        sys.path.insert(0, str(Path(__file__).resolve().parent))
        import soc_aliases  # beside this script

        aliases = soc_aliases.scan(Path(args.aliases_root))
        if aliases:
            payload["aliases"] = aliases
    if args.source != "uboot":
        payload["platforms"] = collect_platforms(
            Path(args.dist) if args.dist else None, assets)
    return payload


def oidc_token(env=os.environ, opener=urllib.request.urlopen) -> str:
    """The job's OIDC token for openipc.org, from the Actions token service."""
    url = env.get("ACTIONS_ID_TOKEN_REQUEST_URL")
    bearer = env.get("ACTIONS_ID_TOKEN_REQUEST_TOKEN")
    if not url or not bearer:
        raise SystemExit("::error::no OIDC token service; the job needs `permissions: id-token: write`")
    sep = "&" if "?" in url else "?"
    req = urllib.request.Request(
        f"{url}{sep}audience={urllib.parse.quote(AUDIENCE, safe='')}",
        headers={"Authorization": f"bearer {bearer}", "Accept": "application/json"},
    )
    with opener(req, timeout=30) as resp:
        return json.load(resp)["value"]


def push(payload: dict, base_url: str, env=os.environ,
         opener=urllib.request.urlopen, sleep=time.sleep) -> int:
    """POST the build. Retries network errors and 5xx; a 4xx is final."""
    body = gzip.compress(json.dumps(payload, separators=(",", ":")).encode())
    url = base_url.rstrip("/") + "/api/v1/builds"
    delay = FIRST_DELAY
    for attempt in range(1, ATTEMPTS + 1):
        try:
            req = urllib.request.Request(url, data=body, method="POST", headers={
                "Authorization": f"Bearer {oidc_token(env, opener)}",
                "Content-Type": "application/json",
                "Content-Encoding": "gzip",
                "User-Agent": "OpenIPC build push",
            })
            with opener(req, timeout=120) as resp:
                print(f"pushed {payload['build']['id']}: HTTP {resp.status} {resp.read().decode(errors='replace')}")
                return 0
        except urllib.error.HTTPError as e:
            text = e.read().decode(errors="replace")
            if 400 <= e.code < 500:
                print(f"::error::openipc.org refused the push: HTTP {e.code} {text}")
                return 1
            reason = f"HTTP {e.code} {text}"
        except (urllib.error.URLError, TimeoutError, ConnectionError) as e:
            reason = str(e)
        if attempt == ATTEMPTS:
            print(f"::error::push failed after {ATTEMPTS} attempts: {reason}")
            return 1
        print(f"::warning::push attempt {attempt} failed ({reason}); retrying in {delay}s")
        sleep(delay)
        delay *= 2
    return 1


def parse_args(argv: list[str]) -> argparse.Namespace:
    p = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    p.add_argument("--source", required=True, choices=["firmware", "builder", "uboot"])
    p.add_argument("--build-id", required=True)
    p.add_argument("--release", help="release tag the assets download from (default: the build id)")
    p.add_argument("--sha", required=True)
    p.add_argument("--built-at", required=True)
    p.add_argument("--webui-digest", default="")
    p.add_argument("--reports", "--dist", dest="dist",
                   help="directory holding the size report and kconfig files")
    p.add_argument("--release-json", help="`gh release view <tag> --json assets` output")
    p.add_argument("--match", help="regex selecting the release assets to push")
    p.add_argument("--assets", action="append", default=[],
                   help="glob of local published files; repeatable (uboot)")
    p.add_argument("--aliases-root", help="repository root to scan for BR2_OPENIPC_SOC_ALIASES")
    p.add_argument("--url", default=os.environ.get("OPENIPC_ORG_URL") or DEFAULT_URL)
    p.add_argument("--out", help="write the payload here instead of pushing it")
    return p.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    payload = build_payload(args, dt.datetime.now(dt.timezone.utc))
    print(f"{payload['build']['id']}: {len(payload['assets'])} asset(s), "
          f"{len(payload.get('platforms', []))} platform(s), {len(payload.get('aliases', {}))} alias(es)")
    if args.out:
        Path(args.out).write_text(json.dumps(payload, indent=1))
        return 0
    return push(payload, args.url)


if __name__ == "__main__":
    sys.exit(main())
