#!/usr/bin/env python3
"""
Tests for general/scripts/kconfig_graph.py.

Run via `python3 general/scripts/tests/test_kconfig_graph.py`. The synthetic
Kconfig tree below sidesteps the real Buildroot Config.in (which is 8k+ symbols
and depends on a checked-out Buildroot source). What we exercise here is the
shape of the emitter's output: types, prompt/non-prompt filtering, currently-set
filter, depends_on / selects / selected_by lists, package attribution from the
Config.in filename, help-text split.

`kconfig_graph.build_report()` is imported as a function; we set the env vars
it expects, point its srctree at a tempdir, and assert on the in-memory
return value (and on the files it wrote to IMAGES_DIR).
"""

from __future__ import annotations

import json
import os
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))


# Synthetic Kconfig tree:
#
#   BR2_PACKAGE_FOO        -- user-prompted, currently set
#   BR2_PACKAGE_BAR        -- user-prompted, currently set, selected by FOO
#   BR2_PACKAGE_BAZ        -- user-prompted, currently NOT set
#   BR2_PACKAGE_QUUX       -- internal (no prompt) "_SUPPORTS"-style, set
#   BR2_PACKAGE_LIBFOO     -- depended on by FOO, currently set
#
#   Expected emit (currently-set ∧ user-prompted):
#     - BR2_PACKAGE_FOO     (selects BR2_PACKAGE_BAR, depends on BR2_PACKAGE_LIBFOO)
#     - BR2_PACKAGE_BAR     (selected_by=[BR2_PACKAGE_FOO])
#     - BR2_PACKAGE_LIBFOO
#
#   BAZ is dropped (not set); QUUX is dropped (no prompt).
SYNTH_CONFIG_IN = """
config BR2_PACKAGE_FOO
\tbool "foo package"
\tdepends on BR2_PACKAGE_LIBFOO
\tselect BR2_PACKAGE_BAR
\thelp
\t  helpful foo

config BR2_PACKAGE_BAR
\tbool "bar package"
\thelp
\t  helpful bar

config BR2_PACKAGE_BAZ
\tbool "baz package"
\thelp
\t  helpful baz

config BR2_PACKAGE_QUUX
\tbool
\tdefault y

config BR2_PACKAGE_LIBFOO
\tbool "libfoo"
\thelp
\t  helpful libfoo
"""

SYNTH_DOT_CONFIG = """\
BR2_PACKAGE_FOO=y
BR2_PACKAGE_BAR=y
# BR2_PACKAGE_BAZ is not set
BR2_PACKAGE_QUUX=y
BR2_PACKAGE_LIBFOO=y
"""


class KconfigGraphTest(unittest.TestCase):
    def setUp(self) -> None:
        self.tmpdir = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmpdir.cleanup)
        root = Path(self.tmpdir.name)

        # Lay out the fake Buildroot tree so build_report()'s default paths
        # resolve.
        self.br2_output_dir = root / "br_output"
        self.br_ver = "2024.02.10"
        srctree = self.br2_output_dir / f"buildroot-{self.br_ver}"
        # Synthesised "package/" tree under srctree so the package_of()
        # attribution can pull a name from the file path.
        pkg_dir = srctree / "package" / "fakepkg"
        pkg_dir.mkdir(parents=True)
        (pkg_dir / "Config.in").write_text(SYNTH_CONFIG_IN)
        (srctree / "Config.in").write_text(
            f'source "{pkg_dir / "Config.in"}"\n'
        )
        (self.br2_output_dir / ".config").write_text(SYNTH_DOT_CONFIG)
        # build_report uses TARGET_DIR only to satisfy its env contract.
        (self.br2_output_dir / "target").mkdir()
        # openipc_defconfig is referenced by setup_kconfig_env defaults.
        (self.br2_output_dir / "openipc_defconfig").write_text("")

        self.images_dir = root / "images"
        self.images_dir.mkdir()

        self.env_snapshot = os.environ.copy()
        for k in (
            "TARGET_DIR",
            "BR2_OUTPUT_DIR",
            "IMAGES_DIR",
            "OPENIPC_SOC_MODEL",
            "OPENIPC_VARIANT",
            "BR_VER",
            "PWD",
            "CONFIG_",
            "BR2_SKIP_LEGACY",
            "srctree",
            "BR2_BASE_DIR",
            "BR2_EXTERNAL",
            "BR2_EXTERNAL_GENERAL_PATH",
            "BR2_EXTERNAL_GENERAL_DESC",
        ):
            os.environ.pop(k, None)
        os.environ["TARGET_DIR"] = str(self.br2_output_dir / "target")
        os.environ["BR2_OUTPUT_DIR"] = str(self.br2_output_dir)
        os.environ["IMAGES_DIR"] = str(self.images_dir)
        os.environ["OPENIPC_SOC_MODEL"] = "testboard"
        os.environ["OPENIPC_VARIANT"] = "testvariant"
        os.environ["BR_VER"] = self.br_ver
        os.environ["PWD"] = str(root)

    def tearDown(self) -> None:
        os.environ.clear()
        os.environ.update(self.env_snapshot)

    def test_emits_graph_and_help_with_filters(self) -> None:
        import kconfig_graph  # local import; env vars are now in place

        graph, helps = kconfig_graph.build_report()

        self.assertEqual(graph["schema"], kconfig_graph.SCHEMA)
        self.assertEqual(graph["board"], "testboard")
        self.assertEqual(graph["variant"], "testvariant")

        names = set(graph["symbols"])
        # Set ∧ user-prompted → in.  Not-set OR not-prompted → out.
        self.assertEqual(
            names,
            {"BR2_PACKAGE_FOO", "BR2_PACKAGE_BAR", "BR2_PACKAGE_LIBFOO"},
            f"unexpected emit set: {sorted(names)}",
        )

        foo = graph["symbols"]["BR2_PACKAGE_FOO"]
        bar = graph["symbols"]["BR2_PACKAGE_BAR"]
        libfoo = graph["symbols"]["BR2_PACKAGE_LIBFOO"]

        self.assertEqual(foo["package"], "fakepkg")
        self.assertEqual(foo["type"], "bool")
        self.assertEqual(foo["prompt"], "foo package")
        self.assertIn("BR2_PACKAGE_LIBFOO", foo["depends_on"])
        self.assertIn("BR2_PACKAGE_BAR", foo["selects"])

        # BAR is selected by FOO; emitter must filter selected_by to
        # currently-set selectors so the cascade UX is accurate.
        self.assertEqual(bar["selected_by"], ["BR2_PACKAGE_FOO"])

        # LIBFOO is only depended on, not selected.
        self.assertEqual(libfoo["selects"], [])
        self.assertEqual(libfoo["selected_by"], [])

        # Help text split: present for each emitted symbol with help.
        self.assertEqual(set(helps["help"]), names)
        self.assertIn("helpful foo", helps["help"]["BR2_PACKAGE_FOO"])

    def test_writes_files_to_images_dir(self) -> None:
        import kconfig_graph  # noqa: F401 ensures module is loaded fresh

        kconfig_graph.build_report()

        graph_path = self.images_dir / "kconfig-graph.testboard-testvariant.json"
        help_path = self.images_dir / "kconfig-help.testboard-testvariant.json"
        self.assertTrue(graph_path.is_file())
        self.assertTrue(help_path.is_file())

        graph = json.loads(graph_path.read_text())
        helps = json.loads(help_path.read_text())
        self.assertEqual(graph["schema"], kconfig_graph.SCHEMA)
        self.assertEqual(helps["schema"], kconfig_graph.SCHEMA)
        self.assertEqual(set(graph["symbols"]), set(helps["help"]))


if __name__ == "__main__":
    unittest.main(verbosity=2)
