<!--
Review standards for this repository live in best_practices.md and
pr_compliance_checklist.yaml at the repo root. Reading them first is the fastest
way to get a PR merged. CLAUDE.md (also readable as AGENTS.md) is the shorter
orientation, and is what an AI assistant loads automatically.

Not every change needs every section — a typo fix does not need an oscilloscope
trace. But a change that alters what runs on a camera needs all four.
-->

## Problem

<!--
The symptom, and which board(s) show it. "Improves stability" is not a symptom.
If this adds new hardware support, say which SoC and which sensor instead.
-->

## Hardware tested on

<!--
SoC and board, e.g. "gk7205v200, Xiaomi Dafang". CI proves an image builds; only
your board can prove it boots and streams.

If you have not run this on a camera, the PR is not ready — say so here and open
it as a draft rather than leaving the section blank.

Unless the change cannot alter what the firmware does on a camera — documentation, review
configuration, repository metadata, this template, CODEOWNERS, or CI machinery
that only selects, lints or tests. If you are unsure, assume it can. Then say so
plainly and give the evidence that does apply — a self-test, selector output, a
parse run. Never paste output from a board that was not exercising the change.

Three that look exempt and are not: a defconfig CI does not build is still a
real image, a workflow feeding a build input changes image bytes, and the
post-build scripts under general/scripts/ rewrite every rootfs.
-->

## Evidence

<!--
Before and after: logs, dmesg, ipcinfo output, stream behaviour, image size.
Paste the output. A description of the output is not the output.
-->

Before:

```

```

After:

```

```

## Scope

<!-- Tick only what you have actually checked. An unticked box is fine; a wrongly ticked one is not. -->

- [ ] No kernel patches under `general/package/all-patches/linux/` (those go to [OpenIPC/linux](https://github.com/OpenIPC/linux))
- [ ] No files specific to a single retail camera model (those go to [OpenIPC/builder](https://github.com/OpenIPC/builder))
- [ ] No probing or bring-up tooling (that goes to [OpenIPC/ipctool](https://github.com/OpenIPC/ipctool))
- [ ] Nothing under `general/overlay/` or in a shared `load_<vendor>` script hardcodes a value specific to my board
- [ ] Package sources come from an OpenIPC repository, and any version bump keeps at least the specificity of the pin it replaces (a new package should pin a full 40-character SHA)
- [ ] No `LD_PRELOAD`, and no binaries that cannot be rebuilt from source
- [ ] New code is selected by a defconfig, so CI actually builds it
