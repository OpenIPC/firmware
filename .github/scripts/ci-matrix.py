#!/usr/bin/env python3
"""Pick the build matrix for a CI run from the set of files a PR touches.

Nearly every change here lands in one SoC family, but the matrix was a fixed
list of 96 boards, so a one-line fix to a load_hisilicon script that reaches
three cameras also rebuilt SigmaStar, Ingenic, Rockchip and Xiongmai. That is
~2000 runner-minutes to re-prove what the last nightly already proved.

The rule is deliberately one-directional: a path has to be *recognised* as
belonging to one SoC before it can narrow anything. Shared code, the overlay,
the build system and anything this script has never heard of all widen back out
to the full matrix. Getting the classification wrong therefore costs runner
time, never coverage.

The path -> board mapping is derived from the tree rather than written down,
because Buildroot already encodes it: a defconfig lists the BR2_PACKAGE_*
symbols it enables, Config.in adds `select` edges, and the .mk files add
_DEPENDENCIES edges. Both edge kinds are load-bearing --- no defconfig sets
BR2_PACKAGE_SIGMASTAR_OSDRV_SENSORS (the infinity6* Config.in files select it)
and nothing selects uclibc-compat (hisilicon-osdrv-hi3516cv100.mk depends on
it), so dropping either kind would resolve a real package to zero boards.

Usage:
    ci-matrix.py               # read GitHub Actions env; append to $GITHUB_OUTPUT
                               # when it is set, otherwise print the same lines
    ci-matrix.py --stdin       # read a file list on stdin, print the decision
    ci-matrix.py --self-test   # check this file still agrees with the tree
"""

import argparse
import collections
import glob
import json
import os
import re
import sys
import urllib.error
import urllib.request

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# The build matrix. This is the single copy --- .github/workflows/build.yml
# reads it from here rather than carrying its own list, so adding a board is
# one edit. Every entry must have a br-ext-chip-*/configs/<board>_defconfig;
# --self-test enforces that.
ALL_BOARDS = [
    # Sigmastar [I6]
    "ssc325_lite", "ssc325de_lite",
    # Sigmastar [I6B]
    "ssc333_lite", "ssc335_lite", "ssc335de_lite", "ssc337_lite", "ssc337de_lite",
    # Sigmastar [I6C]
    "ssc377_lite", "ssc377d_lite", "ssc377de_lite", "ssc377qe_lite",
    "ssc378de_lite", "ssc378qe_lite",
    # Sigmastar [I6E]
    "ssc30kd_lite", "ssc30kq_lite", "ssc338q_lite",
    # Ingenic
    "t10_lite", "t20_lite", "t21_lite", "t30_lite", "t23_lite", "t31_lite", "t40_lite",
    # Hisilicon [HI3516AV100]
    "hi3516av100_lite", "hi3516av100_neo", "hi3516dv100_lite",
    # Hisilicon [HI3516CV100]
    "hi3516cv100_lite", "hi3516cv100_neo", "hi3518cv100_lite", "hi3518ev100_lite",
    # Hisilicon [HI3516CV200]
    "hi3516cv200_lite", "hi3516cv200_neo", "hi3518ev200_lite",
    # Hisilicon [HI3516CV300]
    "hi3516cv300_lite", "hi3516cv300_neo", "hi3516ev100_lite",
    # Hisilicon [HI3516CV500]
    "hi3516av300_lite", "hi3516av300_neo", "hi3516cv500_lite", "hi3516dv300_lite",
    # Hisilicon [HI3516CV6XX]
    "hi3516cv6xx_ultimate",
    # Hisilicon [HI3519DV500]
    "hi3519dv500_ultimate",
    # Hisilicon [HI3516EV200]
    "hi3516dv200_lite", "hi3516ev200_lite", "hi3516ev300_lite", "hi3518ev300_lite",
    "hi3516ev300_neo",
    # Hisilicon [HI3519V101]
    "hi3516av200_lite", "hi3516av200_neo", "hi3519v101_lite",
    # Hisilicon [misc]
    "hi3520dv200_lite", "hi3536cv100_lite", "hi3536dv100_lite",
    # Goke [GK710X]
    "gk7102_lite", "gk7102s_lite",
    # Goke [GK7205V200]
    # gk7205v210 is firmware-identical to gk7205v200 --- built once and served
    # via gk7205v200's BR2_OPENIPC_SOC_ALIASES (manifest @alias).
    "gk7202v300_lite", "gk7205v200_lite", "gk7205v300_lite", "gk7605v100_lite",
    # Goke [GK7205V500]
    "gk7205v500_lite",
    # Allwinner
    "v851s_lite",
    # Fullhan
    "fh8852v100_lite", "fh8852v200_lite",
    # Grainmedia
    "gm8135_lite", "gm8136_lite",
    # Novatek
    "nt98562_lite", "nt98566_lite",
    # Rockchip
    "rv1103_lite", "rv1106_lite", "rv1109_lite", "rv1126_lite",
    # Xiongmai
    # xm550 is firmware-identical to xm530 --- built once and served via
    # xm530's BR2_OPENIPC_SOC_ALIASES (manifest @alias).
    "xm510_lite", "xm530_lite",
    # Ultimate
    "ssc333_ultimate", "ssc335_ultimate", "ssc335de_ultimate", "ssc337_ultimate",
    "ssc337de_ultimate", "ssc30kd_ultimate", "ssc30kq_ultimate", "ssc338q_ultimate",
    "t20_ultimate", "t21_ultimate", "t31_ultimate", "t40_ultimate",
    "hi3516av100_ultimate", "hi3516dv100_ultimate", "hi3518ev200_ultimate",
    "hi3516cv300_ultimate", "hi3516ev200_ultimate", "hi3516ev300_ultimate",
    "hi3518ev300_ultimate", "hi3516av200_ultimate",
    "gk7202v300_ultimate", "gk7205v200_ultimate", "gk7205v300_ultimate",
]

# Workflows that cannot change what a firmware image contains. Matched on the
# whole filename, never as a prefix: a workflow this list has never heard of is
# unknown, and unknown widens. Skipping the matrix for something that does feed
# the build is the one direction this must never fail in.
NO_BUILD_WORKFLOWS = {
    "build-one.yml", "cleanup.yml", "gcc-compat.yml", "image.yml", "manifest.yml",
    "qodo-gate.yml", "shell-tests.yml", "toolchain.yml", "uboot.yml",
}

# Same for .github/scripts/.
NO_BUILD_SCRIPTS = {
    "enrich_manifest.py", "test_load_hisilicon.sh", "test_shell_parse.sh",
    "test_sysupgrade.sh",
}

# CI plumbing: it decides how the build runs but cannot change a byte of what
# ends up on a camera, so what needs proving is that the steps still work, not
# that 96 images still come out the same. These get SMOKE_BOARDS.
#
# ci-matrix.py is deliberately NOT here. Adding a board is an edit to
# ALL_BOARDS, and a smoke set would build the 13 boards that were already there
# and never the new one. It widens like anything else this file has not
# classified.
SMOKE_WORKFLOWS = {"build.yml"}
SMOKE_SCRIPTS = {"check_target_modules.sh"}

# One board per way the build can differ: every architecture, every toolchain
# tuple, every rootfs shape, every variant and every vendor directory appears at
# least once. --self-test enforces exactly that, so this list can be re-picked
# freely as long as the cover still holds -- which is the property that matters,
# not the particular boards.
#
# This is the one place coverage is deliberately traded for runner time: a
# build.yml edit that breaks only a board outside this set reaches master, and
# the nightly finds it within a day rather than the PR finding it in half an
# hour. Everything that can change image content still builds in full.
SMOKE_BOARDS = [
    "hi3516ev200_lite",       # HiSilicon V4, the most-built family here
    "hi3516ev300_ultimate",   # ultimate, and squashfs beside UBI
    "hi3516av300_neo",        # the neo variant, and the gnueabi toolchain
    "hi3519dv500_ultimate",   # the only aarch64 board
    "gk7205v200_lite",        # Goke
    "ssc338q_lite",           # SigmaStar
    "t31_lite",               # Ingenic, the only mips
    "rv1126_lite",            # Rockchip, and UBI with no squashfs
    "gm8136_lite",            # GrainMedia, the only uclibc toolchain
    "nt98566_lite",           # Novatek
    "fh8852v200_lite",        # Fullhan
    "v851s_lite",             # Allwinner
    "xm530_lite",             # Xiongmai
]

# Board directories with no board in ALL_BOARDS. Their defconfigs exist but are
# not built, so a change there narrows to nothing --- which is correct, and is
# listed rather than inferred so that a family silently falling out of the
# matrix fails --self-test instead of quietly skipping CI.
UNBUILT_FAMILIES = {
    ("br-ext-chip-allwinner", "v83x"),
    ("br-ext-chip-ambarella", "s3l"),
    ("br-ext-chip-anyka", "ak3918ev300"),
    ("br-ext-chip-fullhan", "fh8833v100"),
    ("br-ext-chip-sigmastar", "infinity3"),
    ("br-ext-chip-ti", "dm36x"),
}

# Packages no board in ALL_BOARDS builds: enabled only by the 29 defconfigs the
# matrix leaves out, or by the variants that live in OpenIPC/builder. Frozen so
# that a package dropping off the matrix is a --self-test failure rather than a
# silent narrowing. This list is only ever read by --self-test: changes to these
# packages widen like any other path that reaches no board of its own, because
# their .mk and Config.in are included in every build regardless.
NOT_BUILT = {
    "adaptive-link", "aic8800-openipc", "allwinner-osdrv-v83x", "atbm-wifi",
    "aura-httpd", "baresip-openipc", "comgt", "f2fs-tools-openipc", "faceter-agent",
    "faceter-detector", "fdk-aac-openipc", "ffmpeg-openipc", "gdbserver-lite",
    "go2rtc", "herald", "hisi-gpio", "hisilicon-osdrv-hi3536dv100", "i2c-telemetry",
    "libhv-openipc", "libre-openipc", "libsrt-openipc", "linux-patcher", "matter",
    "mavfwd", "mdnsd-openipc", "mini", "mqtt-bot", "msposd", "n3n-openipc", "nabto",
    "netblink", "node-exporter", "ntfy", "onvif-simple-server", "openipc-nfs-root",
    "osd-openipc", "rtl8188eus-openipc", "rtl8192eu-openipc", "rtl8811cu-openipc",
    "rtl8812au", "rtl8812au-openipc", "rtl88x2eu-openipc", "rtw-hostapd", "rubyfpv",
    "siproxd-openipc", "ssv615x-openipc", "ssv635x-openipc", "txw8301-openipc",
    "uqmi-openipc", "vdec-openipc", "venc-openipc", "w1-ds18b20", "webface",
    "webrtc-audio-processing-openipc", "wifibroadcast-ng", "wq9001", "yaml-cli-multi",
}

# Directories under general/package/ that hold no Buildroot package: all-patches
# and all-patches-neo are BR2_GLOBAL_PATCH_DIR and apply to every build (a
# SoC-specific patch dropped in there once broke the whole matrix at once),
# busybox holds the busybox.config every defconfig points at, gcc is copied into
# Buildroot by `make toolchain`, and legacy nests another package tree. None has
# a Config.in, so the "no symbols -> widen" rule already covers them;
# --self-test pins that so a Config.in appearing in one is noticed.
PACKAGE_NON_PACKAGES = {"all-patches", "all-patches-neo", "busybox", "gcc", "legacy"}

# LICENSE and README are anchored to a whole filename. Unanchored they would
# also match LICENSES/vendor.txt and READMEgenerator.c, dropping real code into
# the docs bucket and skipping CI for it entirely.
DOCS = re.compile(r"^(?:docs/|contrib/|(?:LICENSE|README)(?:\.[^/]*)?$)")
# Markdown is only documentation when it lives outside the tree that builds. A
# .md under general/ or br-ext-chip-* falls through to the rules below and is
# treated as whatever it sits next to; costing a few builds is the safe way to
# be wrong.
MARKDOWN = re.compile(r"^(?!general/|br-ext-chip-).*\.md$")
# Configuration for the code-review service, which reads these on the PR and
# never on a build host. Anchored to whole paths for the same reason LICENSE is.
REVIEW_CONFIG = re.compile(r"^(?:\.pr_agent\.toml|pr_compliance_checklist\.yaml)$")
GITHUB_META = re.compile(r"^\.github/(?:CODEOWNERS|PULL_REQUEST_TEMPLATE\.md|ISSUE_TEMPLATE/)")
WORKFLOW = re.compile(r"^\.github/workflows/([^/]+)$")
GITHUB_SCRIPT = re.compile(r"^\.github/scripts/([^/]+)$")
DEFCONFIG = re.compile(r"^br-ext-chip-[^/]+/configs/(.+)_defconfig$")
BOARD_DIR = re.compile(r"^(br-ext-chip-[^/]+)/board/([^/]+)/")
PACKAGE_DIR = re.compile(r"^general/package/([^/]+)/")

# Label that forces the full matrix on a PR, for when you do not trust the
# narrowing --- an opensdk bump behaving oddly, say.
FULL_LABEL = "ci:full"


# --------------------------------------------------------------------------
# The tree: which boards build which packages
# --------------------------------------------------------------------------

class Tree:
    """Buildroot's own package graph, read off the checkout.

    A board builds a package when the package's symbol is in the closure of the
    board's defconfig under Config.in `select` and .mk _DEPENDENCIES edges.
    """

    def __init__(self, root=REPO_ROOT):
        self.root = root
        self._read_packages()
        self._read_defconfigs()
        self._resolve()

    def _read_packages(self):
        self.symbols_of = {}      # package dir -> symbols it declares
        self.package_of = {}      # symbol -> package dir
        self.selects = collections.defaultdict(set)   # symbol -> symbols
        for path in sorted(glob.glob(f"{self.root}/general/package/*/Config.in")):
            package = path.split(os.sep)[-2]
            current, owned = None, []
            with open(path) as handle:
                for line in handle:
                    declared = re.match(r"\s*(?:menu)?config\s+(BR2_\w+)", line)
                    if declared:
                        current = declared.group(1)
                        owned.append(current)
                        self.package_of[current] = package
                        continue
                    selected = re.match(r"\s*select\s+(BR2_\w+)", line)
                    if selected and current:
                        self.selects[current].add(selected.group(1))
            self.symbols_of[package] = owned

        # _DEPENDENCIES names a package directory, not a symbol, and the ones
        # that matter here are guarded: hisilicon-opensdk.mk depends on each
        # osdrv only `ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_...),y)`, purely to
        # order the install. Honouring that guard is what keeps an osdrv change
        # at its own family instead of all 43 HiSilicon boards. Any other
        # conditional is opaque to this parser, so the edge is taken
        # unconditionally --- over-approximating widens, which is the safe way.
        self.depends = collections.defaultdict(list)   # package -> [(guard|None, package)]
        for path in sorted(glob.glob(f"{self.root}/general/package/*/*.mk")):
            package = path.split(os.sep)[-2]
            with open(path) as handle:
                body = re.sub(r"\\\n", " ", handle.read())
            guards = []
            for line in body.splitlines():
                line = line.strip()
                on_symbol = re.match(r"ifeq\s*\(\$\((BR2_\w+)\),y\)", line)
                if on_symbol:
                    guards.append(on_symbol.group(1))
                    continue
                if re.match(r"if(?:eq|neq|def|ndef)\b", line):
                    guards.append(None)
                    continue
                if line.startswith("endif"):
                    if guards:
                        guards.pop()
                    continue
                assignment = re.match(
                    r"(?:HOST_)?[A-Z0-9_]+_DEPENDENCIES\s*[+:]?=\s*(.*)$", line)
                if not assignment:
                    continue
                guard = guards[-1] if guards else None
                for name in assignment.group(1).split():
                    if name in self.symbols_of and name != package:
                        self.depends[package].append((guard, name))

    def _read_defconfigs(self):
        self.boards = {}
        for path in sorted(glob.glob(f"{self.root}/br-ext-chip-*/configs/*_defconfig")):
            name = os.path.basename(path)[: -len("_defconfig")]
            with open(path) as handle:
                body = handle.read()
            family = re.search(r'^BR2_OPENIPC_SOC_FAMILY="([^"]*)"', body, re.M)
            self.boards[name] = {
                "vendor_dir": path[len(self.root) + 1:].split(os.sep)[0],
                "family": family.group(1) if family else "",
                "symbols": set(re.findall(r"^(BR2_PACKAGE_[A-Z0-9_]+)=y", body, re.M)),
                "traits": self._traits(path, body),
            }

    def _traits(self, path, body):
        """What makes a board different to the build steps, not to a camera.

        SMOKE_BOARDS has to span all of these; --self-test checks that it does.
        """
        def string(option):
            found = re.search(rf'^{option}="([^"]*)"', body, re.M)
            return found.group(1) if found else "?"

        if re.search(r"^BR2_aarch64=y", body, re.M):
            architecture = "aarch64"
        elif re.search(r"^BR2_mips", body, re.M):
            architecture = "mips"
        else:
            architecture = "arm"
        # UBI vs squashfs decides which images `make repack` emits and so which
        # of build.yml's `find output/images -name openipc*no{r,and}*` fires.
        rootfs = ",".join(kind for kind in ("CPIO", "SQUASHFS", "UBI")
                          if re.search(rf"^BR2_TARGET_ROOTFS_{kind}=y", body, re.M))
        return {
            "vendor:" + path[len(self.root) + 1:].split(os.sep)[0][len("br-ext-chip-"):],
            "arch:" + architecture,
            "toolchain:" + string("BR2_TOOLCHAIN_EXTERNAL_CUSTOM_PREFIX"),
            "rootfs:" + rootfs,
            "variant:" + string("BR2_OPENIPC_VARIANT"),
        }

    def _closure(self, seed):
        symbols, packages, growing = set(seed), set(), True
        while growing:
            growing = False
            for symbol in list(symbols):
                for selected in self.selects.get(symbol, ()):
                    if selected not in symbols:
                        symbols.add(selected)
                        growing = True
                owner = self.package_of.get(symbol)
                if owner and owner not in packages:
                    packages.add(owner)
                    growing = True
            for package in list(packages):
                for guard, dependency in self.depends.get(package, ()):
                    if guard is not None and guard not in symbols:
                        continue
                    if dependency not in packages:
                        packages.add(dependency)
                        growing = True
                        for symbol in self.symbols_of.get(dependency, ()):
                            if symbol not in symbols:
                                symbols.add(symbol)
                                growing = True
        return packages

    def _resolve(self):
        self.built = [b for b in ALL_BOARDS if b in self.boards]
        self.smoke = [b for b in self.built if b in set(SMOKE_BOARDS)]
        for board in self.built:
            self.boards[board]["packages"] = self._closure(self.boards[board]["symbols"])

    def boards_for_package(self, package):
        return [b for b in self.built if package in self.boards[b]["packages"]]

    def boards_for_family(self, vendor_dir, family):
        return [b for b in self.built
                if self.boards[b]["vendor_dir"] == vendor_dir
                and self.boards[b]["family"] == family]


# --------------------------------------------------------------------------
# Classification
# --------------------------------------------------------------------------

def classify(tree, changed, labels=(), event="pull_request", draft=False):
    """Map a list of changed paths to {rows, needs_build, reason}."""
    full = list(tree.built)

    if event != "pull_request":
        return _decision(full, True, reason=f"{event} builds everything")
    if FULL_LABEL in labels:
        return _decision(full, True, reason=f"{FULL_LABEL} label set")
    if draft:
        return _decision([], False, reason="draft pull request")
    if not changed:
        return _decision(full, True, reason="no file list available")

    boards, smoked = set(), False
    for path in changed:
        if DOCS.match(path) or MARKDOWN.match(path) or REVIEW_CONFIG.match(path):
            continue
        if GITHUB_META.match(path):
            continue
        workflow = WORKFLOW.match(path)
        if workflow:
            if workflow.group(1) in NO_BUILD_WORKFLOWS:
                continue
            if workflow.group(1) in SMOKE_WORKFLOWS:
                boards.update(tree.smoke)
                smoked = True
                continue
            return _decision(full, True, reason=f"{path} affects every board")
        script = GITHUB_SCRIPT.match(path)
        if script:
            if script.group(1) in NO_BUILD_SCRIPTS:
                continue
            if script.group(1) in SMOKE_SCRIPTS:
                boards.update(tree.smoke)
                smoked = True
                continue
            return _decision(full, True, reason=f"{path} affects every board")

        defconfig = DEFCONFIG.match(path)
        if defconfig:
            # A defconfig outside ALL_BOARDS is never built, so it contributes
            # nothing --- the matrix, not the tree, decides what CI covers.
            if defconfig.group(1) in tree.built:
                boards.add(defconfig.group(1))
            continue

        board_dir = BOARD_DIR.match(path)
        if board_dir:
            hits = tree.boards_for_family(board_dir.group(1), board_dir.group(2))
            if hits:
                boards.update(hits)
                continue
            if (board_dir.group(1), board_dir.group(2)) in UNBUILT_FAMILIES:
                continue
            return _decision(full, True, reason=f"{path} is an unknown SoC family")

        package_dir = PACKAGE_DIR.match(path)
        if package_dir:
            hits = tree.boards_for_package(package_dir.group(1))
            if hits:
                boards.update(hits)
                continue
            # No board in the matrix enables this package -- but it still
            # widens, and NOT_BUILT deliberately does not shortcut that.
            # general/external.mk does
            #     include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
            # and general/package/Config.in sources every Config.in, both
            # unconditionally, so a syntax error in a package nothing enables
            # still breaks make or kconfig for all 96 boards. Narrowing to zero
            # here would let that reach master unbuilt.
            return _decision(full, True, reason=f"{path} reaches no board of its own")

        # The overlay, the Makefile, general/scripts, general/linux, the
        # Buildroot fragment, a vendor Config.in, this file --- or something
        # new. Widen.
        return _decision(full, True, reason=f"{path} affects every board")

    if not boards:
        return _decision([], False, reason="nothing that reaches a build")
    if not smoked:
        return _decision(sorted(boards), True, reason="narrowed to the affected boards")
    # Say so out loud. These runs are the one case where a green matrix does not
    # mean every board was proven, and a log line reading "narrowed to the
    # affected boards" would misrepresent that.
    if boards == set(tree.smoke):
        return _decision(sorted(boards), True,
                         reason="CI plumbing only: smoke set, not every board")
    return _decision(sorted(boards), True,
                     reason="affected boards plus the CI-plumbing smoke set")


def _decision(rows, needs_build, reason):
    return {
        # Actions rejects an empty include list outright, so an empty matrix is
        # expressed by skipping the whole job via needs_build instead.
        "matrix": {"include": [{"platform": b} for b in rows] or [{"platform": "none"}]},
        "needs_build": needs_build,
        "rows": rows,
        "reason": reason,
    }


# --------------------------------------------------------------------------
# GitHub plumbing
# --------------------------------------------------------------------------

def changed_files_from_api():
    """Ask the API which files the PR touches.

    Cheaper and more robust than a diff: computing a merge base needs history
    the build jobs otherwise only ever clone shallowly.
    """
    repo = os.environ.get("GITHUB_REPOSITORY")
    token = os.environ.get("GH_TOKEN") or os.environ.get("GITHUB_TOKEN")
    event = _event()
    number = event.get("pull_request", {}).get("number")
    if not (repo and token and number):
        return None

    files, page = [], 1
    while True:
        url = (f"https://api.github.com/repos/{repo}/pulls/{number}/files"
               f"?per_page=100&page={page}")
        request = urllib.request.Request(url, headers={
            "Authorization": f"Bearer {token}",
            "Accept": "application/vnd.github+json",
        })
        with urllib.request.urlopen(request, timeout=30) as response:
            batch = json.load(response)
        files += [entry["filename"] for entry in batch]
        # A rename drops the old path from the list; it may be the only thing
        # tying the change to a SoC, so count it too.
        files += [entry["previous_filename"] for entry in batch
                  if entry.get("previous_filename")]
        if len(batch) < 100:
            return files
        page += 1
        # The endpoint stops at 3000 files. Returning what we have would be a
        # partial list, and a partial list can only ever narrow the matrix too
        # far --- so say we do not know, which the caller reads as "build
        # everything".
        if page > 30:
            print("ci-matrix: PR exceeds the 3000-file listing limit", file=sys.stderr)
            return None


def _event():
    path = os.environ.get("GITHUB_EVENT_PATH")
    if not (path and os.path.exists(path)):
        return {}
    with open(path) as handle:
        return json.load(handle)


# --------------------------------------------------------------------------
# Self-test
# --------------------------------------------------------------------------

def self_test():
    """Fail loudly when this file drifts from the tree it describes."""
    tree = Tree()
    problems = []

    # 1. Every board we claim to build must have a defconfig to build from.
    for board in ALL_BOARDS:
        if board not in tree.boards:
            problems.append(f"{board} is in ALL_BOARDS but has no defconfig")

    # 2. Every board directory must either back a board in the matrix or be
    #    named as one we knowingly do not build.
    for path in sorted(glob.glob(f"{REPO_ROOT}/br-ext-chip-*/board/*")):
        if not os.path.isdir(path):
            continue
        vendor_dir, family = path.split(os.sep)[-3], path.split(os.sep)[-1]
        if tree.boards_for_family(vendor_dir, family):
            continue
        if (vendor_dir, family) not in UNBUILT_FAMILIES:
            problems.append(
                f"{vendor_dir}/board/{family}/ backs no board in ALL_BOARDS; add it "
                f"to UNBUILT_FAMILIES if that is deliberate")

    # 3. Same contract for packages. This is the check that catches a defconfig
    #    or a _DEPENDENCIES edit quietly taking a package out of the matrix.
    for path in sorted(glob.glob(f"{REPO_ROOT}/general/package/*/")):
        package = path.rstrip(os.sep).split(os.sep)[-1]
        if package in PACKAGE_NON_PACKAGES:
            if tree.symbols_of.get(package):
                problems.append(
                    f"general/package/{package}/ now declares a Config.in symbol; it is "
                    f"listed in PACKAGE_NON_PACKAGES and would stop widening")
            continue
        if not tree.symbols_of.get(package):
            problems.append(
                f"general/package/{package}/ has no Config.in symbol; add it to "
                f"PACKAGE_NON_PACKAGES if it is not a Buildroot package")
            continue
        if tree.boards_for_package(package):
            continue
        if package not in NOT_BUILT:
            problems.append(
                f"general/package/{package}/ is built by no board in ALL_BOARDS; add "
                f"it to NOT_BUILT if that is deliberate")
    for package in sorted(NOT_BUILT):
        if tree.boards_for_package(package):
            problems.append(
                f"{package} is in NOT_BUILT but the matrix builds it now; drop it")

    # 4. The smoke set has to be real boards, has to be smaller than the thing
    #    it stands in for, and has to span every way the build can differ --
    #    otherwise it is not a smoke test, it is a guess.
    for board in SMOKE_BOARDS:
        if board not in tree.boards:
            problems.append(f"{board} is in SMOKE_BOARDS but has no defconfig")
        elif board not in tree.built:
            problems.append(f"{board} is in SMOKE_BOARDS but not in ALL_BOARDS")
    # classify() consults the no-build lists first, so a name in both is dead
    # config: it reads as "smoke-tested" and builds nothing. Safe, but a lie.
    for both, where in [(NO_BUILD_WORKFLOWS & SMOKE_WORKFLOWS, "workflow"),
                        (NO_BUILD_SCRIPTS & SMOKE_SCRIPTS, "script")]:
        for name in sorted(both):
            problems.append(
                f"{name} is both a no-build and a smoke {where}; the no-build "
                f"list wins and the smoke entry never fires")
    # A renamed or deleted entry stops matching and silently widens. Harmless,
    # but it is a list of names that no longer describe anything.
    for names, directory in [(NO_BUILD_WORKFLOWS | SMOKE_WORKFLOWS, "workflows"),
                             (NO_BUILD_SCRIPTS | SMOKE_SCRIPTS, "scripts")]:
        for name in sorted(names):
            if not os.path.exists(os.path.join(REPO_ROOT, ".github", directory, name)):
                problems.append(
                    f".github/{directory}/{name} is classified but does not exist")
    if len(tree.smoke) >= len(tree.built):
        problems.append("SMOKE_BOARDS is not smaller than the full matrix")
    covered = set().union(*(tree.boards[b]["traits"] for b in tree.smoke)) \
        if tree.smoke else set()
    for trait in sorted(set().union(*(tree.boards[b]["traits"] for b in tree.built))):
        if trait not in covered:
            problems.append(
                f"no board in SMOKE_BOARDS has {trait}; a CI-plumbing change "
                f"would go unproven for it")

    # 5. The narrowing must be sound: what is not recognised as one SoC's own
    #    has to come back with the full matrix, and what is has to come back
    #    with exactly the boards that build it.
    full = len(tree.built)
    smoke = len(tree.smoke)
    cases = [
        # Vendor packages narrow to the families that enable them.
        (["general/package/hisilicon-osdrv-hi3516ev200/files/script/load_hisilicon"],
         8, "osdrv narrows to its family"),
        (["general/package/hisilicon-opensdk/hisilicon-opensdk.mk"],
         43, "opensdk spans HiSilicon and Goke"),
        (["general/package/goke-osdrv-gk7205v200/Config.in"], 7, "goke osdrv"),
        (["general/package/hisilicon-osdrv-hi3520dv200/files/script/load_hisilicon"],
         1, "single-board osdrv"),
        # The two edge kinds the closure depends on. uclibc-compat is reached
        # only through a .mk _DEPENDENCIES line and sigmastar-osdrv-sensors only
        # through a Config.in select; either one missing zeroes them out.
        (["general/package/uclibc-compat/src/uclibc-compat-static.c"],
         3, "reached via .mk _DEPENDENCIES"),
        (["general/package/sigmastar-osdrv-sensors/Config.in"],
         24, "reached via Config.in select"),
        # Shared packages narrow too, just barely.
        (["general/package/majestic/majestic.mk"], 86, "majestic is nearly everywhere"),
        # Board configs and kernel configs.
        (["br-ext-chip-goke/configs/gk7205v200_lite_defconfig"], 1, "one defconfig"),
        (["br-ext-chip-hisilicon/board/hi3516ev200/hi3516ev300.generic.config"],
         8, "kernel config narrows to its family"),
        (["general/package/hisilicon-osdrv-hi3516cv200/files/script/load_hisilicon",
          "br-ext-chip-hisilicon/configs/hi3516cv200_lite_defconfig"],
         3, "union of two narrowing paths"),
        # Everything shared, global, or unknown widens.
        (["Makefile"], full, "build system"),
        (["general/openipc.fragment"], full, "fragment appended to every defconfig"),
        (["general/overlay/etc/init.d/rcS"], full, "overlay ships in every rootfs"),
        (["general/scripts/rootfs_script.sh"], full, "post-build hook"),
        (["general/package/Config.in"], full, "package registry"),
        (["general/package/all-patches/gcc/11.4.0/0001-libsanitizer-musl-compat.patch"],
         full, "BR2_GLOBAL_PATCH_DIR applies to every build"),
        (["general/package/busybox/busybox.config"], full, "every defconfig points at it"),
        # external.mk includes every package's .mk and Config.in sources every
        # Config.in, so a package nothing enables can still break every board.
        (["general/package/rtl8188eus-openipc/rtl8188eus-openipc.mk"],
         full, "a package no board enables still widens"),
        (["general/package/legacy/datalink/files/tweaksys"], full, "the nested legacy tree"),
        (["br-ext-chip-hisilicon/external.mk"], full, "vendor external tree"),
        ([".github/scripts/ci-matrix.py"], full,
         "this file picks the boards, so it cannot pick fewer for itself"),
        ([".github/workflows/some-new-thing.yml"], full, "an unknown workflow widens"),
        ([".github/scripts/some-new-thing.sh"], full, "an unknown script widens"),
        # CI plumbing gets the smoke set: it cannot change image content, only
        # whether the steps still run.
        ([".github/workflows/build.yml"], smoke, "the build workflow smoke-tests"),
        ([".github/scripts/check_target_modules.sh"], smoke, "the module check"),
        ([".github/workflows/build.yml", ".github/workflows/qodo-gate.yml"],
         smoke, "smoke plus something that never builds"),
        ([".github/workflows/build.yml", "general/overlay/etc/passwd"],
         full, "smoke loses to anything that changes an image"),
        ([".github/workflows/build.yml", ".github/scripts/ci-matrix.py"],
         full, "smoke loses to the selector itself"),
        ([".github/workflows/build.yml", "general/package/goke-osdrv-gk7205v200/x"],
         len(set(tree.smoke) | set(tree.boards_for_package("goke-osdrv-gk7205v200"))),
         "smoke unions with a narrowed package"),
        (["general/package/hisilicon-osdrv-hi3516ev200/x", "general/overlay/etc/passwd"],
         full, "one shared path widens the whole set"),
        # Nothing that reaches a build.
        (["README.md"], 0, "readme"),
        (["docs/anything.md"], 0, "docs"),
        (["LICENSE"], 0, "licence"),
        ([".pr_agent.toml"], 0, "review agent config"),
        (["pr_compliance_checklist.yaml"], 0, "compliance checklist"),
        (["best_practices.md"], 0, "review standards are markdown"),
        (["CLAUDE.md"], 0, "agent instructions are markdown"),
        ([".github/workflows/qodo-gate.yml"], 0, "review gate never builds"),
        ([".github/scripts/test_sysupgrade.sh"], 0, "shell-tests fixture"),
        ([".github/PULL_REQUEST_TEMPLATE.md"], 0, "PR template"),
        (["contrib/openipc-bisect/openipc-bisect"], 0, "developer tooling"),
        ([".pr_agent.toml", "docs/architecture.md"], 0, "review config plus docs"),
        (["best_practices.md",
          "general/package/goke-osdrv-gk7205v200/Config.in"], 7, "rules plus SoC code"),
        # ...and the same names anywhere else, or with anything appended, are
        # not them. Unanchored these would skip CI for whatever they really are.
        (["LICENSES/vendor.txt"], full, "LICENSES/ is not the licence file"),
        (["READMEgenerator.c"], full, "README prefix is not a readme"),
        (["general/package/majestic/README.md"],
         86, "markdown inside a package is that package"),
        (["br-ext-chip-hisilicon/board/hi3516ev200/NOTES.md"],
         8, "markdown inside a board dir is that family"),
        (["general/scripts/pr_compliance_checklist.yaml"],
         full, "same name under general/ is not the checklist"),
        ([".pr_agent.toml.orig"], full, "a merge leftover is not the config"),
        (["tools/.pr_agent.toml"], full, "same name in a subdirectory is not the config"),
    ]
    for paths, expected, what in cases:
        got = len(classify(tree, paths)["rows"])
        if got != expected:
            problems.append(f"{what}: expected {expected} rows, got {got}")

    # 6. Zero rows and "go build something" must never be emitted together, in
    #    either direction: Actions errors on an empty include list, and a row
    #    list nobody builds is a matrix that silently proves nothing.
    for paths, _, what in cases:
        decision = classify(tree, paths)
        if decision["needs_build"] and not decision["rows"]:
            problems.append(f"{what}: needs_build with an empty matrix")
        if decision["rows"] and not decision["needs_build"]:
            problems.append(f"{what}: rows with needs_build false")

    # 7. Non-PR events and the escape hatch always get everything.
    for kwargs, what in [({"event": "schedule"}, "schedule"),
                         ({"event": "workflow_dispatch"}, "dispatch"),
                         ({"labels": [FULL_LABEL]}, FULL_LABEL)]:
        if len(classify(tree, ["README.md"], **kwargs)["rows"]) != full:
            problems.append(f"{what} must build everything")
    if classify(tree, ["general/package/majestic/majestic.mk"], draft=True)["needs_build"]:
        problems.append("a draft pull request must not build")

    # 8. No workflow may declare a top-level key twice. This is legal YAML ---
    #    the last one wins, which is why a local lint waves it through --- but
    #    Actions rejects the file outright and the run fails with no jobs at
    #    all. Squash-merging a stacked branch is how one appears: the same block
    #    added by two commits with different SHAs is two independent additions
    #    to git, and it keeps both.
    workflows = os.path.join(REPO_ROOT, ".github", "workflows")
    for name in sorted(os.listdir(workflows)):
        if not name.endswith((".yml", ".yaml")):
            continue
        seen = set()
        with open(os.path.join(workflows, name)) as handle:
            for line in handle:
                key = re.match(r"([A-Za-z_][\w-]*):", line)   # column 0 only
                if not key:
                    continue
                if key.group(1) in seen:
                    problems.append(
                        f".github/workflows/{name} declares '{key.group(1)}' more than "
                        f"once; Actions will refuse the file")
                seen.add(key.group(1))

    for problem in problems:
        print(f"ci-matrix: {problem}", file=sys.stderr)
    if problems:
        return 1
    print(f"ci-matrix: self-test ok ({len(tree.built)} boards, "
          f"{len(tree.symbols_of)} packages, {len(cases)} cases)")
    return 0


# --------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--stdin", action="store_true",
                        help="read changed paths from stdin instead of the API")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    tree = Tree()
    if args.stdin:
        changed = [line.strip() for line in sys.stdin if line.strip()]
        event, labels, draft = "pull_request", [], False
    else:
        event = os.environ.get("GITHUB_EVENT_NAME", "pull_request")
        pull_request = _event().get("pull_request", {})
        labels = [label["name"] for label in pull_request.get("labels", [])]
        draft = bool(pull_request.get("draft"))
        changed = None
        if event == "pull_request":
            try:
                changed = changed_files_from_api()
            except (urllib.error.URLError, OSError, ValueError) as exc:
                # Never fail the run over this; an empty list means full matrix.
                print(f"ci-matrix: cannot list PR files ({exc})", file=sys.stderr)

    decision = classify(tree, changed or [], labels, event, draft)

    print(f"ci-matrix: {len(decision['rows'])}/{len(tree.built)} boards "
          f"(needs_build={decision['needs_build']}) --- {decision['reason']}",
          file=sys.stderr)
    for board in decision["rows"]:
        print(f"  {board}", file=sys.stderr)

    lines = [
        f"matrix={json.dumps(decision['matrix'], separators=(',', ':'))}",
        f"needs-build={str(decision['needs_build']).lower()}",
        f"reason={decision['reason']}",
    ]
    # Write $GITHUB_OUTPUT here rather than having the workflow redirect stdout
    # into it. Under a redirect every print() in this file is one keystroke away
    # from corrupting the step outputs, and a crash between the first line and
    # the last leaves a half-written file that Actions still reads. Diagnostics
    # go to stderr precisely so that cannot happen -- which is a rule that has
    # to hold forever to stay safe, instead of a property of the code.
    # Falls back to stdout so a local run still shows what it decided.
    destination = None if args.stdin else os.environ.get("GITHUB_OUTPUT")
    if destination:
        with open(destination, "a") as handle:
            handle.write("\n".join(lines) + "\n")
    else:
        print("\n".join(lines))
    return 0


if __name__ == "__main__":
    sys.exit(main())
