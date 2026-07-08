#!/usr/bin/env python3
"""
Per-board Kconfig dependency graph emitter for the firmware-explorer
configurator (firmware-explorer v0.3).

Reads environment variables exported by the top-level Makefile's `kconfig-graph`
target (the same env contract size_report.py uses), instantiates kconfiglib
against the merged Buildroot tree, and writes two schema-versioned JSON files
into IMAGES_DIR:

    kconfig-graph.<board>-<variant>.json   - symbol nodes, dependency edges,
                                             currently_set / user_can_disable
    kconfig-help.<board>-<variant>.json    - per-symbol help text, split out
                                             because it's ~70% of the bytes and
                                             changes rarely

Splitting help text is a deliberate storage win — the explorer ships the help
once per source rather than per build × platform. See plan section "Help text
bloat" for the math.

Required env:
  TARGET_DIR              final assembled rootfs (mirrors size_report.py contract)
  BR2_OUTPUT_DIR          Buildroot output dir (has buildroot-<ver>/, .config, …)
  IMAGES_DIR              dir to write the JSON outputs into
  OPENIPC_SOC_MODEL       e.g. hi3518ev300
  OPENIPC_VARIANT         lite | ultimate | neo
  BR_VER                  Buildroot version dir under BR2_OUTPUT_DIR
                          (defaults to 2024.02.10)
  PWD                     repo root; used to compute BR2_EXTERNAL when not
                          already set

Optional env (let the caller override the auto-derived defaults):
  BR2_EXTERNAL            defaults to <PWD>/general
  BR2_EXTERNAL_GENERAL_PATH  defaults to <PWD>/general
  BR2_EXTERNAL_GENERAL_DESC  defaults to a generic Buildroot external description
  BR2_DEFCONFIG           defaults to <BR2_OUTPUT_DIR>/openipc_defconfig

Run after `make BOARD=...` (defconfig must already be merged).
"""

from __future__ import annotations

import json
import os
import sys
from pathlib import Path

SCHEMA = 1
SYMBOL_PREFIX = "BR2_PACKAGE_"


def env(name: str, *, required: bool = True, default: str | None = None) -> str:
    val = os.environ.get(name, default)
    if required and not val:
        sys.exit(f"kconfig_graph: missing required env var {name}")
    return val or ""


def package_of(filename: str | None, *, repo_root: str) -> str | None:
    """
    Map the Config.in file that defines a symbol to a Buildroot package name.

    The robust mapping is: take the parent directory of the Config.in file and
    use its basename. Works for both Buildroot core packages (package/<name>/
    Config.in) and OpenIPC external packages (general/package/<name>/Config.in
    or general/package/legacy/<name>/Config.in).

    Returns None when the symbol is defined outside a package/ tree (toolchain,
    fs, system, …).
    """
    if not filename:
        return None
    parts = Path(filename).parts
    # We look for the last "package" segment and take the next path element.
    for i, p in enumerate(parts):
        if p == "package" and i + 1 < len(parts):
            return parts[i + 1]
    return None


def setup_kconfig_env(br2_output_dir: str, br_ver: str, repo_root: str) -> str:
    """
    Configure the environment kconfiglib needs to evaluate Buildroot's Kconfig
    tree. Returns the srctree (the buildroot source dir) the caller should chdir
    into so relative `source` directives resolve correctly.

    Buildroot's top-level Config.in references $BR2_BASE_DIR and
    $BR2_EXTERNAL_*_PATH at parse time. Mirrors the env Buildroot's own Makefile
    exports during defconfig.
    """
    srctree = os.path.join(br2_output_dir, f"buildroot-{br_ver}")

    defaults = {
        "BR2_BASE_DIR": br2_output_dir,
        "BR2_DEFCONFIG": os.path.join(br2_output_dir, "openipc_defconfig"),
        "BR2_EXTERNAL": os.path.join(repo_root, "general"),
        "BR2_EXTERNAL_GENERAL_PATH": os.path.join(repo_root, "general"),
        "BR2_EXTERNAL_GENERAL_DESC": "Buildroot external tree for general packages",
        "srctree": srctree,
        # Buildroot symbols are already prefixed with `BR2_` in the symbol
        # table; .config lines look like `BR2_FOO=y`, with no additional
        # prefix. Kconfiglib's default `CONFIG_` prefix would silently
        # strip nothing useful and skip every load — set it explicitly
        # empty so loaded values land where we expect.
        "CONFIG_": "",
        # Skip Config.in.legacy parsing. It only declares "renamed/removed"
        # warnings that we don't surface in the configurator UI and slows
        # the load by a few hundred ms.
        "BR2_SKIP_LEGACY": "YES",
    }
    for k, v in defaults.items():
        os.environ.setdefault(k, v)
    return srctree


def build_report() -> tuple[dict, dict]:
    target_dir = env("TARGET_DIR")  # noqa: F841 (kept to match size_report contract)
    br2_output_dir = env("BR2_OUTPUT_DIR")
    images_dir = env("IMAGES_DIR")
    soc_model = env("OPENIPC_SOC_MODEL")
    variant = env("OPENIPC_VARIANT")
    br_ver = env("BR_VER", required=False, default="2024.02.10")
    repo_root = env("PWD", required=False, default=os.getcwd())

    srctree = setup_kconfig_env(br2_output_dir, br_ver, repo_root)

    import kconfiglib  # imported late so missing-dep errors are scoped here

    config_in = os.path.join(srctree, "Config.in")
    if not os.path.isfile(config_in):
        sys.exit(f"kconfig_graph: Config.in not found at {config_in}")

    cwd = os.getcwd()
    try:
        os.chdir(srctree)
        kc = kconfiglib.Kconfig("Config.in", warn_to_stderr=False)
        dotconfig = os.path.join(br2_output_dir, ".config")
        if not os.path.isfile(dotconfig):
            sys.exit(f"kconfig_graph: .config not found at {dotconfig}")
        kc.load_config(dotconfig)
    finally:
        os.chdir(cwd)

    type_to_str = kconfiglib.TYPE_TO_STR
    expr_str = kconfiglib.expr_str

    def _walk_expr(expr) -> list[str]:
        """Collect all symbol names referenced anywhere in a Kconfig expression."""
        out: list[str] = []

        def rec(e):
            if isinstance(e, kconfiglib.Symbol):
                if e.name and e.name != "y" and e.name != "n":
                    out.append(e.name)
            elif isinstance(e, tuple):
                for child in e[1:]:
                    rec(child)

        rec(expr)
        return sorted(set(out))

    # Pre-build a reverse-select index: target_symbol -> [symbols that `select`
    # it]. Walking `sym.rev_dep` directly collects symbols from the condition
    # expression too (`depends on X` referenced in a `select target if X`),
    # which is wrong for the "what hard-pins this on" UX — only direct
    # selectors should appear in `selected_by`.
    reverse_selects: dict[str, list[str]] = {}
    for s in kc.unique_defined_syms:
        if not s.name:
            continue
        for tgt, _cond in s.selects:
            if not tgt.name:
                continue
            reverse_selects.setdefault(tgt.name, []).append(s.name)

    # Only emit currently-set BR2_PACKAGE_* symbols that have a user-facing
    # prompt in menuconfig — those are the real configurable knobs on this
    # board. Filter rationale:
    #
    # 1. Currently-set only: this is the "what can I disable to free space"
    #    UX. Buildroot defines ~7k symbols total; only ~100 are set per board.
    #    Adding new packages would require pulling in their entire dep tree
    #    plus possibly conflicting with vendor-side requirements — out of
    #    scope for v0.3.
    # 2. Prompted only: drops *_ARCH_SUPPORTS / *_SUPPORTS / HAS_* and other
    #    feature-detection symbols Buildroot derives automatically. Those
    #    aren't user-toggleable in any UI and would just be noise in the
    #    configurator.
    # 3. The currently-set set is dep-closed by Kconfig itself: every
    #    symbol referenced from an emitted symbol's depends_on / select chain
    #    is also set (otherwise the parent wouldn't satisfy its deps), so the
    #    explorer can resolve cascades without missing references.
    def _is_prompted(sym: "kconfiglib.Symbol") -> bool:
        return bool(sym.nodes) and any(n.prompt for n in sym.nodes)

    package_syms: list[kconfiglib.Symbol] = [
        s
        for s in kc.unique_defined_syms
        if s.name
        and s.name.startswith(SYMBOL_PREFIX)
        and s.tri_value > 0
        and _is_prompted(s)
    ]

    graph_symbols: dict[str, dict] = {}
    help_text: dict[str, str] = {}
    skipped_no_node = 0

    for sym in package_syms:
        if not sym.nodes:
            skipped_no_node += 1
            continue
        node = sym.nodes[0]

        # Buildroot's `assignable` is "could the user write 0 here with no
        # other changes" — too strict for our UX, because the explorer's
        # closeDisable will cascade-disable selectors. We instead expose
        # `selected_by` filtered to currently-set selectors so the UI can
        # tell the user "to disable X you must also disable Y".
        selects: list[str] = sorted({tgt.name for tgt, _cond in sym.selects if tgt.name})
        selected_by_active = sorted(
            n
            for n in reverse_selects.get(sym.name, [])
            if (kn := kc.syms.get(n))
            and kn.tri_value > 0
            and n != sym.name
        )

        depends_on: list[str] = sorted(
            n for n in _walk_expr(sym.direct_dep) if n != sym.name
        )

        # `prompt_text` is the short label Buildroot shows in menuconfig
        # ("majestic" / "Wireguard kernel module" / …). Cheap to embed for
        # the picker UI, and helps when symbol names don't trivially map
        # to a recognisable feature.
        prompts = [n.prompt[0] for n in sym.nodes if n.prompt]

        graph_symbols[sym.name] = {
            "package": package_of(node.filename, repo_root=repo_root),
            "type": type_to_str.get(sym.orig_type, "unknown"),
            "prompt": prompts[0] if prompts else None,
            "depends_on": depends_on,
            "selects": selects,
            "selected_by": selected_by_active,
            "direct_dep_expr": expr_str(sym.direct_dep),
        }

        if node.help:
            help_text[sym.name] = node.help

    graph = {
        "schema": SCHEMA,
        "board": soc_model,
        "variant": variant,
        "br_ver": br_ver,
        "symbol_prefix": SYMBOL_PREFIX,
        "symbol_count": len(graph_symbols),
        "skipped_no_node": skipped_no_node,
        "symbols": graph_symbols,
    }
    helps = {
        "schema": SCHEMA,
        "board": soc_model,
        "variant": variant,
        "help": help_text,
    }

    Path(images_dir).mkdir(parents=True, exist_ok=True)
    graph_path = Path(images_dir) / f"kconfig-graph.{soc_model}-{variant}.json"
    help_path = Path(images_dir) / f"kconfig-help.{soc_model}-{variant}.json"
    graph_path.write_text(json.dumps(graph, indent=2) + "\n")
    help_path.write_text(json.dumps(helps, indent=2) + "\n")

    cascading = sum(
        1 for s in graph_symbols.values() if s["selected_by"]
    )
    print(
        f"kconfig-graph: {graph_path}\n"
        f"  symbols={len(graph_symbols)} (currently-set, user-prompted) "
        f"cascade_required={cascading} skipped_no_node={skipped_no_node}\n"
        f"  help: {help_path} ({len(help_text)} entries)"
    )
    return graph, helps


def main() -> None:
    build_report()


if __name__ == "__main__":
    main()
