#!/usr/bin/env python3
"""Tests for push_build.py: what it pushes, and how it talks to openipc.org.

  python3 .github/scripts/test_push_build.py
"""
from __future__ import annotations

import datetime as dt
import gzip
import hashlib
import io
import json
import sys
import tempfile
import unittest
import urllib.error
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import push_build  # noqa: E402
import soc_aliases  # noqa: E402

NOW = dt.datetime(2026, 9, 26, 18, 51, 28, tzinfo=dt.timezone.utc)


def args(tmp: Path, *extra: str):
    return push_build.parse_args([
        "--source", "firmware", "--build-id", "nightly-20260925-230295e",
        "--sha", "230295e494013e17a9802633a58b30ed7c937f8c",
        "--built-at", "2026-09-25T17:48:37Z", "--dist", str(tmp),
        "--assets", str(tmp / "openipc.*.tgz"), *extra,
    ])


class Payload(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.dist = Path(self._tmp.name)
        (self.dist / "openipc.gk7205v200-nor-lite.tgz").write_bytes(b"lite image")
        (self.dist / "openipc.gk7205v200-nor-ultimate.tgz").write_bytes(b"ultimate")
        (self.dist / "openipc.ssc338q-nand-lite.tgz").write_bytes(b"nand")
        (self.dist / "sizes.gk7205v200-lite.json").write_text(
            json.dumps({"schema": 1, "board": "gk7205v200", "variant": "lite", "flash_mb": 8}))
        (self.dist / "kconfig-graph.gk7205v200-lite.json").write_text(json.dumps({"schema": 1, "symbols": {}}))
        (self.dist / "kconfig-help.gk7205v200-lite.json").write_text(json.dumps({"schema": 1, "help": {}}))

    def tearDown(self):
        self._tmp.cleanup()

    def test_build_and_assets(self):
        p = push_build.build_payload(args(self.dist), NOW)
        self.assertEqual(p["schema"], 1)
        self.assertEqual(p["source"], "firmware")
        self.assertEqual(p["build"], {
            "id": "nightly-20260925-230295e", "release": "nightly-20260925-230295e",
            "sha": "230295e494013e17a9802633a58b30ed7c937f8c",
            "built_at": "2026-09-25T17:48:37Z", "published_at": "2026-09-26T18:51:28Z"})
        names = [a["name"] for a in p["assets"]]
        self.assertEqual(names, sorted(names))
        lite = next(a for a in p["assets"] if a["name"] == "openipc.gk7205v200-nor-lite.tgz")
        self.assertEqual(lite["size"], len(b"lite image"))
        self.assertEqual(lite["sha256"], hashlib.sha256(b"lite image").hexdigest())
        # The sidecars are pushed as platform detail, never as assets.
        self.assertFalse(any(n.endswith(".json") for n in names))

    def test_platforms_carry_their_documents_and_survive_a_missing_report(self):
        p = push_build.build_payload(args(self.dist), NOW)
        plats = {x["name"]: x for x in p["platforms"]}
        self.assertEqual(sorted(plats), ["gk7205v200-lite", "gk7205v200-ultimate", "ssc338q-lite"])
        self.assertEqual(plats["gk7205v200-lite"]["sizes"]["flash_mb"], 8)
        self.assertIn("kconfig_graph", plats["gk7205v200-lite"])
        self.assertIn("kconfig_help", plats["gk7205v200-lite"])
        # No size report for these two: still listed, without detail.
        self.assertEqual(plats["gk7205v200-ultimate"], {"name": "gk7205v200-ultimate"})
        self.assertEqual(plats["ssc338q-lite"], {"name": "ssc338q-lite"})

    def test_an_unreadable_report_costs_its_detail_not_the_push(self):
        (self.dist / "sizes.gk7205v200-ultimate.json").write_text("{not json")
        p = push_build.build_payload(args(self.dist), NOW)
        plats = {x["name"]: x for x in p["platforms"]}
        self.assertNotIn("sizes", plats["gk7205v200-ultimate"])

    def test_nothing_published_is_an_error(self):
        empty = Path(self._tmp.name) / "empty"
        empty.mkdir()
        with self.assertRaises(SystemExit):
            push_build.build_payload(args(empty), NOW)

    def test_webui_digest_and_release(self):
        p = push_build.build_payload(args(self.dist, "--webui-digest", "sha256:ab", "--release", "latest"), NOW)
        self.assertEqual(p["build"]["webui_digest"], "sha256:ab")
        self.assertEqual(p["build"]["release"], "latest")

    def test_uboot_has_no_platforms(self):
        (self.dist / "u-boot-t31-universal-nor.bin").write_bytes(b"u-boot")
        a = push_build.parse_args([
            "--source", "uboot", "--build-id", "uboot-20260926T120000Z-abcdef0", "--release", "latest",
            "--sha", "a" * 40, "--built-at", "2026-09-26T12:00:00Z",
            "--assets", str(self.dist / "u-boot-*.bin")])
        p = push_build.build_payload(a, NOW)
        self.assertNotIn("platforms", p)
        self.assertEqual([x["name"] for x in p["assets"]], ["u-boot-t31-universal-nor.bin"])


class Aliases(unittest.TestCase):
    def test_scan(self):
        with tempfile.TemporaryDirectory() as t:
            root = Path(t)
            cfgs = root / "br-ext-chip-goke" / "configs"
            cfgs.mkdir(parents=True)
            (cfgs / "gk7205v200_lite_defconfig").write_text(
                'BR2_OPENIPC_SOC_MODEL="gk7205v200"\nBR2_OPENIPC_SOC_ALIASES="gk7205v210 gk7205v200"\n')
            (cfgs / "gk7205v300_lite_defconfig").write_text(
                'BR2_OPENIPC_SOC_MODEL="gk7205v300"\nBR2_OPENIPC_SOC_ALIASES="gk7205v210"\n')
            (cfgs / "gk7202v300_lite_defconfig").write_text('BR2_OPENIPC_SOC_MODEL="gk7202v300"\n')
            # First model in sorted order keeps a contested chip; a model is
            # never its own alias; a defconfig without aliases adds nothing.
            self.assertEqual(soc_aliases.scan(root), {"gk7205v210": "gk7205v200"})

    def test_the_real_tree_parses(self):
        root = Path(__file__).resolve().parents[2]
        self.assertTrue(all(k and v for k, v in soc_aliases.scan(root).items()))


class Response(io.BytesIO):
    def __init__(self, status: int, body: bytes):
        super().__init__(body)
        self.status = status

    def __enter__(self):
        return self

    def __exit__(self, *a):
        return False


class Transport(unittest.TestCase):
    ENV = {"ACTIONS_ID_TOKEN_REQUEST_URL": "https://token.example/req?api-version=2.0",
           "ACTIONS_ID_TOKEN_REQUEST_TOKEN": "runner-bearer"}
    PAYLOAD = {"build": {"id": "nightly-20260925-230295e"}, "assets": []}

    def opener(self, post_results):
        calls = []
        results = list(post_results)

        def open_(req, timeout=None):
            calls.append(req)
            if req.get_method() == "GET":
                return Response(200, json.dumps({"value": "oidc.jwt"}).encode())
            r = results.pop(0)
            if isinstance(r, Exception):
                raise r
            return Response(r, b'{"build":"nightly-20260925-230295e"}')
        return open_, calls

    def test_token_request_names_openipc_org_as_audience(self):
        open_, calls = self.opener([201])
        self.assertEqual(push_build.oidc_token(self.ENV, open_), "oidc.jwt")
        req = calls[0]
        self.assertEqual(req.full_url,
                         "https://token.example/req?api-version=2.0&audience=https%3A%2F%2Fopenipc.org")
        self.assertEqual(req.get_header("Authorization"), "bearer runner-bearer")

    def test_no_token_service_is_a_clear_error(self):
        with self.assertRaises(SystemExit):
            push_build.oidc_token({}, None)

    def test_post_is_gzip_json_with_the_bearer(self):
        open_, calls = self.opener([201])
        self.assertEqual(push_build.push(self.PAYLOAD, "https://openipc.org/", self.ENV, open_, lambda s: None), 0)
        post = [c for c in calls if c.get_method() == "POST"][0]
        self.assertEqual(post.full_url, "https://openipc.org/api/v1/builds")
        self.assertEqual(post.get_header("Authorization"), "Bearer oidc.jwt")
        self.assertEqual(post.get_header("Content-encoding"), "gzip")
        self.assertEqual(json.loads(gzip.decompress(post.data)), self.PAYLOAD)

    def test_5xx_and_network_errors_are_retried_with_backoff(self):
        err = urllib.error.HTTPError("u", 502, "bad gateway", {}, io.BytesIO(b"upstream"))
        open_, calls = self.opener([err, urllib.error.URLError("reset"), 201])
        slept = []
        self.assertEqual(push_build.push(self.PAYLOAD, "https://openipc.org", self.ENV, open_, slept.append), 0)
        self.assertEqual(slept, [5, 10])
        self.assertEqual(sum(c.get_method() == "POST" for c in calls), 3)

    def test_4xx_is_final(self):
        err = urllib.error.HTTPError("u", 403, "forbidden", {}, io.BytesIO(b'{"error":"workflow not allowed"}'))
        open_, calls = self.opener([err, 201])
        slept = []
        self.assertEqual(push_build.push(self.PAYLOAD, "https://openipc.org", self.ENV, open_, slept.append), 1)
        self.assertEqual(slept, [])

    def test_gives_up_after_five_attempts(self):
        open_, calls = self.opener([urllib.error.URLError("down")] * 5)
        slept = []
        self.assertEqual(push_build.push(self.PAYLOAD, "https://openipc.org", self.ENV, open_, slept.append), 1)
        self.assertEqual(slept, [5, 10, 20, 40])


if __name__ == "__main__":
    unittest.main()
