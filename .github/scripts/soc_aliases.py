"""SoC aliases from the defconfigs: BR2_OPENIPC_SOC_ALIASES.

A published image's BR2_OPENIPC_SOC_MODEL also serves the space-separated
retired or compatible chip ids listed in BR2_OPENIPC_SOC_ALIASES. That lets a
camera still reporting the old id (xm550, gk7205v210, hi3516cv610, ...) be
sent to the image that exists.

Two readers share this module:
  - enrich_manifest.py, for the gh-pages manifest that on-device sysupgrade
    reads;
  - push_build.py, for the build facts pushed to openipc.org.
They read the aliases the same way, so the two cannot disagree.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

SOC_MODEL_RE = re.compile(r'^BR2_OPENIPC_SOC_MODEL\s*=\s*"?([A-Za-z0-9]+)"?\s*$')
SOC_ALIASES_RE = re.compile(r'^BR2_OPENIPC_SOC_ALIASES\s*=\s*"?([^"\n]*)"?\s*$')


def scan(root: Path) -> dict[str, str]:
    """Map each alias chip id to the SOC_MODEL it is published under.

    `root` is the repository root, holding br-ext-chip-*/configs. A chip
    claimed by two models keeps the first, in sorted defconfig order, and the
    conflict is reported on stderr. A missing tree yields {}.
    """
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
