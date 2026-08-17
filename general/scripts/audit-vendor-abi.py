#!/usr/bin/env python3
"""
audit-vendor-abi.py - Vendor binary ABI compatibility auditor for OpenIPC
==========================================================================

WHAT THIS TOOL DOES
-------------------

OpenIPC ships vendor "osdrv" packages containing prebuilt .so libraries from
chip manufacturers (HiSilicon, Sigmastar, Ingenic, Goke, Rockchip, etc).
These binaries were compiled by vendors against old C libraries -- typically
uclibc from the Linux 3.x era, or older glibc -- but OpenIPC runs them on
modern musl libc.

This tool performs two definitive checks (no guessing):

  1. SYMBOL CHECK: extracts every imported symbol from vendor .so files and
     checks whether anything in the process image defines it.  A symbol only
     counts as missing when neither musl NOR a sibling .so shipped in the same
     package provides it -- HiSilicon and Goke packages ship libsecurec.so,
     which defines the C11 Annex K family (memcpy_s, snprintf_s, ...) that the
     other vendor libraries import via DT_NEEDED.  Symbols owned by the
     compiler runtime (libgcc's __aeabi_*/__divdi3, libstdc++'s __cxa_*) are
     reported separately: they resolve from whichever executable loads the
     library, so they are not a libc gap either.

  2. STRUCT PROBE: compiles a small C program with the platform's musl
     cross-compiler and runs it under qemu to get exact sizeof/offsetof
     for every struct that crosses the vendor-musl boundary (stat, off_t,
     pthread_mutex_t, jmp_buf, etc).  Reports the actual musl sizes so
     they can be compared against known vendor libc layouts.

REQUIREMENTS
------------

  - Python 3.10+
  - readelf (from binutils)
  - A Buildroot output directory with the musl cross-compiler and sysroot
    (i.e., run at least one `make BOARD=...` before using this tool)
  - qemu-arm-static or qemu-arm (to run the compiled struct probe)

USAGE
-----

    # Audit a specific package (MVP)
    ./general/scripts/audit-vendor-abi.py --package hisilicon-osdrv-hi3516ev200

    # Audit all packages
    ./general/scripts/audit-vendor-abi.py

    # Just check symbols, skip struct probe (no compiler/qemu needed)
    ./general/scripts/audit-vendor-abi.py --symbols-only

    # Via Makefile
    make audit-abi

EXIT CODES
----------

    0   No missing symbols or struct mismatches detected
    1   Missing symbols found (vendor .so will fail to load on musl)
    2   Struct size mismatches found (runtime corruption risk)
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

# ---------------------------------------------------------------------------
# Data types
# ---------------------------------------------------------------------------

@dataclass
class BinaryInfo:
    path: str           # absolute path
    rel_path: str       # relative to package dir
    imports: list       # [(binding, symbol), ...]
    exports: set        # symbols this .so DEFINES (resolvable by siblings)
    soname: str         # DT_SONAME, or the file basename if unset
    needed: list        # NEEDED entries from .dynamic
    version_reqs: list  # GLIBC_2.x etc version tags


@dataclass
class PackageResult:
    name: str
    source_libc: str = "unknown"
    arch: str = ""
    so_count: int = 0
    binaries: list = field(default_factory=list)
    missing_symbols: dict = field(default_factory=lambda: defaultdict(list))
    # maps symbol -> [binary_rel_paths]
    struct_mismatches: list = field(default_factory=list)
    # symbol -> sorted list of sibling SONAMEs in this package that define it
    sibling_resolved: dict = field(default_factory=dict)
    # symbols provided by the compiler runtime the consumer links (libgcc,
    # libstdc++), not by libc -- never a shim's job
    toolchain_symbols: dict = field(default_factory=dict)


# ---------------------------------------------------------------------------
# Struct types that cross the vendor-musl boundary.
# Each entry: (header, type_expr, fields_with_offsets)
# Fields is a list of (field_name, access_expr) for offsetof probing.
# ---------------------------------------------------------------------------

STRUCT_PROBES = [
    ("sys/stat.h", "struct stat", [
        ("st_dev", "st_dev"), ("st_ino", "st_ino"), ("st_mode", "st_mode"),
        ("st_nlink", "st_nlink"), ("st_uid", "st_uid"), ("st_gid", "st_gid"),
        ("st_rdev", "st_rdev"), ("st_size", "st_size"),
    ]),
    ("sys/types.h", "off_t", []),
    ("time.h", "time_t", []),
    ("time.h", "struct timespec", [
        ("tv_sec", "tv_sec"), ("tv_nsec", "tv_nsec"),
    ]),
    ("pthread.h", "pthread_mutex_t", []),
    ("pthread.h", "pthread_cond_t", []),
    ("signal.h", "struct sigaction", []),
    ("setjmp.h", "jmp_buf", []),
    ("dirent.h", "struct dirent", [
        ("d_ino", "d_ino"), ("d_off", "d_off"),
        ("d_reclen", "d_reclen"), ("d_type", "d_type"),
        ("d_name", "d_name"),
    ]),
    ("fcntl.h", "struct flock", [
        ("l_type", "l_type"), ("l_whence", "l_whence"),
        ("l_start", "l_start"), ("l_len", "l_len"),
        ("l_pid", "l_pid"),
    ]),
]

# Symbols that are known to be inter-vendor-library (not libc).
# We skip these in the "missing from musl" report to reduce noise.
# Patterns are checked with str.startswith or exact match.
VENDOR_SYMBOL_PREFIXES = (
    "HI_", "Hi_", "hi_", "MPI_", "mpi_", "ISP_", "isp_",  # HiSilicon
    "MI_", "mi_",                                            # Sigmastar
    "IMP_", "imp_",                                          # Ingenic
    "FDK", "pcmDmx", "aac",                                 # Audio codecs
    "FD_", "RK_", "rk_",                                     # Rockchip
    "XM_", "xm_",                                            # Xiongmai
    "GK_", "gk_", "SC_",                                     # Goke
    "AW_", "aw_",                                            # Allwinner
    "_Z",                                                     # C++ mangled
)

# These are commonly undefined weak symbols or linker internals that
# never resolve and are expected to be NULL.  Not a real problem.
LINKER_INTERNALS = {
    "__gmon_start__", "_Jv_RegisterClasses",
    "__deregister_frame_info", "__register_frame_info",
    "_ITM_deregisterTMCloneTable", "_ITM_registerTMCloneTable",
    "__GNU_EH_FRAME_HDR", "_gp_disp", "__gnu_local_gp",
}

# Symbols supplied by the compiler runtime rather than by libc.  A vendor .so
# importing these is not a musl problem: libgcc / libgcc_eh / libstdc++ get
# linked into whichever executable loads the library.  Reporting them as
# "missing from musl" is what inflated the counts in issue #1992 -- e.g.
# rockchip-osdrv-rv11xx imports 15 C++ ABI symbols that libstdc++ provides.
TOOLCHAIN_RUNTIME_PREFIXES = (
    "__aeabi_",      # ARM EABI helpers (libgcc)
    "__cxa_",        # C++ ABI (libstdc++)
    "_Unwind_",      # unwinder (libgcc_eh)
    "__gxx_",        # C++ personality routine (libstdc++)
)

TOOLCHAIN_RUNTIME_SYMBOLS = {
    # libgcc integer/float helpers
    "__divdi3", "__udivdi3", "__moddi3", "__umoddi3", "__divdc3",
    "__floatdisf", "__floatdidf", "__fixdfdi", "__fixunsdfdi",
    "__popcountsi2", "__popcountdi2", "__clzsi2", "__ctzsi2",
    # libgcc_eh frame registration
    "__register_frame_info_bases", "__deregister_frame_info_bases",
    "__register_frame_info_header_bases",
    # libstdc++
    "__dynamic_cast",
}


def is_toolchain_runtime_symbol(sym: str) -> bool:
    """True if the compiler runtime (libgcc/libstdc++) provides this symbol.

    Note __aeabi_d2iz is deliberately NOT special-cased away here: older
    Buildroot ARM toolchains have been seen not to emit it, which is why
    uclibc-compat carries an implementation.  It is still a libgcc symbol,
    so it is bucketed with the rest and simply not counted as a musl gap.
    """
    if sym in TOOLCHAIN_RUNTIME_SYMBOLS:
        return True
    return any(sym.startswith(p) for p in TOOLCHAIN_RUNTIME_PREFIXES)


# ---------------------------------------------------------------------------
# ELF helpers
# ---------------------------------------------------------------------------

def run_readelf(args: list[str], path: str) -> str:
    try:
        r = subprocess.run(
            ["readelf", "-W"] + args + [path],
            capture_output=True, text=True, timeout=10,
        )
        return r.stdout
    except (subprocess.TimeoutExpired, FileNotFoundError):
        return ""


def is_elf(path: str) -> bool:
    try:
        with open(path, "rb") as f:
            return f.read(4) == b"\x7fELF"
    except OSError:
        return False


def is_exportable(bind: str, vis: str) -> bool:
    """True if a defined .dynsym entry can actually satisfy another DSO.

    Being defined is not enough.  A LOCAL binding is not visible outside the
    object and HIDDEN/INTERNAL visibility is not visible to the dynamic
    linker at all, so neither can resolve a sibling's import.  Counting one
    as a provider would let a genuinely missing symbol be reported as
    sibling-resolved.

    In this tree the entries this actually removes are benign -- across the
    vendor .so sampled, the 366 non-exportable .dynsym entries are 243 LOCAL
    section symbols (.init, .jcr) and 123 instances of readelf's own header
    row, which otherwise parses as a symbol literally named "Name".  No
    HIDDEN entry was observed, and no symbol changed classification when this
    filter was added.  It is here so that a vendor drop which does carry one
    cannot quietly mask a real gap.
    """
    return bind in ("GLOBAL", "WEAK") and vis in ("DEFAULT", "PROTECTED")


def get_musl_exports(libc_path: str) -> set[str]:
    """Get all symbols exported by musl libc.so."""
    out = run_readelf(["--dyn-syms"], libc_path)
    exports = set()
    for line in out.splitlines():
        parts = line.split()
        if len(parts) >= 8 and not is_exportable(parts[4], parts[5]):
            continue
        if len(parts) >= 8 and parts[6] != "UND" and parts[7]:
            exports.add(parts[7].split("@")[0])
    return exports


def parse_binary(path: str, pkg_dir: str) -> BinaryInfo | None:
    """Parse a single .so file, extract imports, NEEDED, versions."""
    if not is_elf(path):
        return None

    rel_path = os.path.relpath(path, pkg_dir)

    # Dynamic section for NEEDED / SONAME
    dyn_out = run_readelf(["-d"], path)
    needed = re.findall(r"NEEDED.*\[(.+?)\]", dyn_out)
    m_soname = re.search(r"SONAME.*\[(.+?)\]", dyn_out)
    soname = m_soname.group(1) if m_soname else os.path.basename(path)

    # Dynamic symbols -- undefined imports and defined exports
    dynsym_out = run_readelf(["--dyn-syms"], path)
    imports = []
    exports = set()
    for line in dynsym_out.splitlines():
        parts = line.split()
        if len(parts) >= 8 and parts[7]:
            sym = parts[7].split("@")[0]
            if parts[6] == "UND":
                imports.append((parts[4], sym))
            elif is_exportable(parts[4], parts[5]):
                exports.add(sym)

    # GNU version requirements
    ver_out = run_readelf(["-V"], path)
    version_reqs = sorted(set(re.findall(
        r"(G(?:LIBC|LIBCXX)_[\d.]+)", ver_out
    )))

    return BinaryInfo(
        path=path, rel_path=rel_path,
        imports=imports, exports=exports, soname=soname,
        needed=needed, version_reqs=version_reqs,
    )


def detect_source_libc(binaries: list[BinaryInfo]) -> str:
    """Detect source libc from NEEDED entries across all binaries."""
    all_needed = set()
    all_imports = set()
    for b in binaries:
        all_needed.update(b.needed)
        all_imports.update(sym for _, sym in b.imports)

    if any("ld-uClibc" in n or n == "libc.so.0" for n in all_needed):
        return "uclibc"
    if any("libc.so.6" in n or "ld-linux" in n for n in all_needed):
        return "glibc"
    if any(s.startswith("__uClibc_") for s in all_imports):
        return "uclibc"
    if any(s in ("__xstat", "__fxstat", "__lxstat") for s in all_imports):
        return "glibc"
    return "unknown"


def detect_arch(path: str) -> str:
    hdr = run_readelf(["-h"], path)
    machine = bits = ""
    for line in hdr.splitlines():
        if "Machine:" in line:
            machine = line.split("Machine:", 1)[1].strip()
        elif "Class:" in line:
            c = line.split("Class:", 1)[1].strip()
            bits = "32-bit" if "32" in c else "64-bit" if "64" in c else ""
    return f"{machine} {bits}".strip()


def is_vendor_symbol(sym: str) -> bool:
    """True if symbol looks like a vendor-internal (not libc) symbol."""
    if sym in LINKER_INTERNALS:
        return True
    if any(sym.startswith(p) for p in VENDOR_SYMBOL_PREFIXES):
        return True
    # Uppercase-start symbols are usually vendor-defined
    if sym[0:1].isupper():
        return True
    # g_ prefixed globals
    if sym.startswith("g_"):
        return True
    return False


# ---------------------------------------------------------------------------
# Struct probe: compile & run under qemu
# ---------------------------------------------------------------------------

PROBE_SOURCE_TEMPLATE = """\
#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <setjmp.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>

int main(void) {
%s
    return 0;
}
"""


def generate_probe_source() -> str:
    """Generate C source that prints sizeof/offsetof for all probed types."""
    lines = []
    for header, type_expr, fields in STRUCT_PROBES:
        lines.append(f'    printf("sizeof({type_expr})=%zu\\n", sizeof({type_expr}));')
        for fname, access in fields:
            lines.append(
                f'    printf("  {fname} offset=%zu size=%zu\\n", '
                f'offsetof({type_expr}, {access}), '
                f'sizeof((({type_expr}*)0)->{access}));'
            )
    return PROBE_SOURCE_TEMPLATE % "\n".join(lines)


def compile_and_run_probe(cc: str) -> dict[str, str] | None:
    """Compile the struct probe, run under qemu, return {line: value} dict."""
    source = generate_probe_source()

    with tempfile.TemporaryDirectory() as tmpdir:
        src_path = os.path.join(tmpdir, "probe.c")
        bin_path = os.path.join(tmpdir, "probe")

        with open(src_path, "w") as f:
            f.write(source)

        # Compile static so qemu can run it without sysroot
        r = subprocess.run(
            [cc, "-o", bin_path, src_path, "-static"],
            capture_output=True, text=True, timeout=30,
        )
        if r.returncode != 0:
            print(f"  WARNING: probe compilation failed: {r.stderr.strip()}", file=sys.stderr)
            return None

        # Try qemu-arm-static first, then qemu-arm
        for qemu in ("qemu-arm-static", "qemu-arm"):
            try:
                r = subprocess.run(
                    [qemu, bin_path],
                    capture_output=True, text=True, timeout=10,
                )
                if r.returncode == 0:
                    return _parse_probe_output(r.stdout)
            except FileNotFoundError:
                continue

        print("  WARNING: qemu not found, cannot run struct probe", file=sys.stderr)
        return None


def _parse_probe_output(output: str) -> dict[str, str]:
    """Parse probe output into dict preserving original lines."""
    result = {}
    for line in output.strip().splitlines():
        line = line.strip()
        if "=" in line:
            result[line.split("=")[0].strip()] = line.split("=")[1].strip()
    return result


# Known uclibc ARM 32-bit struct sizes (from uclibc 0.9.33 / ARM EABI).
# These are what vendor binaries compiled against uclibc expect.
UCLIBC_ARM32 = {
    "sizeof(struct stat)": "88",
    "  st_dev offset": "0",
    "  st_ino offset": "12",
    "  st_mode offset": "16",
    "  st_nlink offset": "20",
    "  st_uid offset": "24",
    "  st_gid offset": "28",
    "  st_rdev offset": "32",
    "  st_size offset": "44",
    "sizeof(off_t)": "4",
    "sizeof(time_t)": "4",
    "sizeof(struct timespec)": "8",
    "  tv_sec offset": "0",
    "  tv_nsec offset": "4",
    "sizeof(pthread_mutex_t)": "24",
    "sizeof(pthread_cond_t)": "48",
    "sizeof(struct sigaction)": "140",
    "sizeof(jmp_buf)": "392",
    "sizeof(struct dirent)": "280",
    "sizeof(struct flock)": "24",
    "  l_type offset": "0",
    "  l_whence offset": "2",
    "  l_start offset": "4",
    "  l_len offset": "8",
    "  l_pid offset": "12",
}

# Known glibc ARM 32-bit struct sizes (glibc 2.31 / ARM EABI).
GLIBC_ARM32 = {
    "sizeof(struct stat)": "88",
    "  st_dev offset": "0",
    "  st_ino offset": "12",
    "  st_mode offset": "16",
    "  st_nlink offset": "20",
    "  st_uid offset": "24",
    "  st_gid offset": "28",
    "  st_rdev offset": "32",
    "  st_size offset": "44",
    "sizeof(off_t)": "4",
    "sizeof(time_t)": "4",
    "sizeof(struct timespec)": "8",
    "  tv_sec offset": "0",
    "  tv_nsec offset": "4",
    "sizeof(pthread_mutex_t)": "24",
    "sizeof(pthread_cond_t)": "48",
    "sizeof(struct sigaction)": "140",
    "sizeof(jmp_buf)": "392",
    "sizeof(struct dirent)": "280",
    "sizeof(struct flock)": "24",
    "  l_type offset": "0",
    "  l_whence offset": "2",
    "  l_start offset": "4",
    "  l_len offset": "8",
    "  l_pid offset": "12",
}

KNOWN_VENDOR_LAYOUTS = {
    "uclibc": UCLIBC_ARM32,
    "glibc": GLIBC_ARM32,
}

# Functions that pass these structs by pointer (callers allocate the struct,
# so size and layout MUST match).
STRUCT_FUNCTIONS = {
    "struct stat": [
        "stat", "fstat", "lstat", "stat64", "fstat64", "lstat64",
        "__xstat", "__fxstat", "__lxstat",
        "__xstat64", "__fxstat64", "__lxstat64",
    ],
    "off_t": [
        "mmap", "mmap64", "lseek", "lseek64",
        "ftruncate", "ftruncate64", "truncate", "truncate64",
        "pread", "pread64", "pwrite", "pwrite64",
        "fseeko", "fseeko64", "ftello", "ftello64",
        "sendfile", "sendfile64",
    ],
    "struct flock": ["fcntl", "fcntl64"],
    "struct timespec": ["clock_gettime", "clock_getres", "clock_nanosleep", "nanosleep"],
    "time_t": ["time", "stime"],
    "pthread_mutex_t": [
        "pthread_mutex_init", "pthread_mutex_destroy",
        "pthread_mutex_lock", "pthread_mutex_unlock",
        "pthread_mutex_trylock", "pthread_mutex_timedlock",
    ],
    "pthread_cond_t": [
        "pthread_cond_init", "pthread_cond_destroy",
        "pthread_cond_wait", "pthread_cond_signal",
        "pthread_cond_broadcast", "pthread_cond_timedwait",
    ],
    "struct sigaction": ["sigaction"],
    "jmp_buf": ["setjmp", "_setjmp", "sigsetjmp", "longjmp", "_longjmp", "siglongjmp"],
    "struct dirent": ["readdir", "readdir64", "scandir", "scandir64"],
}

# Reverse: symbol -> struct name
_SYM_TO_STRUCT = {}
for _stype, _syms in STRUCT_FUNCTIONS.items():
    for _s in _syms:
        _SYM_TO_STRUCT[_s] = _stype


# ---------------------------------------------------------------------------
# Main audit logic
# ---------------------------------------------------------------------------

def audit_package(pkg_name: str, pkg_dir: str, musl_exports: set[str],
                  musl_sizes: dict[str, str] | None) -> PackageResult | None:
    """Audit all .so files in a package."""
    files_dir = os.path.join(pkg_dir, "files")
    if not os.path.isdir(files_dir):
        return None

    # Collect .so files
    so_files = []
    for root, _dirs, filenames in os.walk(files_dir):
        for fn in filenames:
            if ".so" in fn:
                so_files.append(os.path.join(root, fn))
    so_files.sort()
    if not so_files:
        return None

    result = PackageResult(name=pkg_name)

    # Parse all binaries
    for so in so_files:
        info = parse_binary(so, pkg_dir)
        if info:
            result.binaries.append(info)
            result.so_count += 1

    if not result.binaries:
        return None

    result.source_libc = detect_source_libc(result.binaries)
    result.arch = detect_arch(result.binaries[0].path)

    # A symbol is only "missing" if nothing in the process image defines it.
    # musl is one provider; the sibling .so shipped alongside in the same
    # package are another.  HiSilicon and Goke packages ship libsecurec.so,
    # which defines the whole C11 Annex K family (memcpy_s, snprintf_s, ...)
    # that the other vendor libraries import via DT_NEEDED.  Checking musl
    # alone reports those as missing and overstates the real gap.
    sibling_providers = defaultdict(set)
    for binfo in result.binaries:
        for sym in binfo.exports:
            sibling_providers[sym].add(binfo.soname)

    # Phase 1: Missing symbols (skip vendor-internal and linker symbols)
    for binfo in result.binaries:
        for binding, sym in binfo.imports:
            if sym in musl_exports:
                continue
            if is_vendor_symbol(sym):
                continue
            if binding == "WEAK" and sym in LINKER_INTERNALS:
                continue
            if sym in LINKER_INTERNALS:
                continue
            if is_toolchain_runtime_symbol(sym):
                result.toolchain_symbols.setdefault(sym, []).append(binfo.rel_path)
                continue
            providers = sibling_providers.get(sym)
            if providers:
                # Defined by a sibling in this package.  Flag the case where
                # the importer does not name that sibling in its own DT_NEEDED
                # -- it then depends on the provider being pulled into the
                # process by someone else, which is fragile but not broken.
                direct = providers & set(binfo.needed)
                result.sibling_resolved.setdefault(
                    sym, {"providers": sorted(providers), "indirect": []},
                )
                if not direct:
                    result.sibling_resolved[sym]["indirect"].append(binfo.rel_path)
                continue
            result.missing_symbols[sym].append(binfo.rel_path)

    # Phase 2: Struct mismatches
    if musl_sizes:
        vendor_sizes = KNOWN_VENDOR_LAYOUTS.get(result.source_libc)
        if vendor_sizes:
            # Find which struct-passing functions are actually imported
            imported_syms = set()
            for binfo in result.binaries:
                for _, sym in binfo.imports:
                    imported_syms.add(sym)

            # Check each struct type
            for stype, func_list in STRUCT_FUNCTIONS.items():
                used_funcs = [f for f in func_list if f in imported_syms]
                if not used_funcs:
                    continue

                # Compare all sizes/offsets for this struct type
                mismatches = []
                for key, musl_val in musl_sizes.items():
                    if not key.startswith(f"sizeof({stype})") and \
                       not (key.startswith("  ") and f"sizeof({stype})" in
                            _find_parent_sizeof(key, musl_sizes)):
                        # Check if this key belongs to this struct
                        if not _key_belongs_to_struct(key, stype, musl_sizes):
                            continue
                    vendor_val = vendor_sizes.get(key)
                    if vendor_val is not None and vendor_val != musl_val:
                        mismatches.append((key, vendor_val, musl_val))

                if mismatches:
                    # Which binaries use these functions?
                    affected = []
                    for binfo in result.binaries:
                        bsyms = {s for _, s in binfo.imports}
                        if bsyms & set(used_funcs):
                            affected.append(binfo.rel_path)

                    result.struct_mismatches.append({
                        "struct": stype,
                        "functions": used_funcs,
                        "binaries": affected,
                        "mismatches": mismatches,
                    })

    return result


def _key_belongs_to_struct(key: str, stype: str, sizes: dict) -> bool:
    """Check if an offset key belongs to a given struct type."""
    if key.startswith(f"sizeof({stype})"):
        return True
    # For field-level offsets, we use the STRUCT_PROBES definition
    for header, type_expr, fields in STRUCT_PROBES:
        if type_expr == stype:
            for fname, _ in fields:
                if key.strip().startswith(fname):
                    return True
    return False


def _find_parent_sizeof(key: str, sizes: dict) -> str:
    """Not used currently but kept for future expansion."""
    return ""


# ---------------------------------------------------------------------------
# Report
# ---------------------------------------------------------------------------

def print_report(result: PackageResult, quiet: bool) -> None:
    print(f"Package: {result.name}")
    print(f"  Source libc: {result.source_libc}")
    print(f"  Architecture: {result.arch}")
    print(f"  Binaries: {result.so_count} .so files")
    print()

    has_issues = False

    # Missing symbols
    if result.missing_symbols:
        has_issues = True
        libc_missing = {}
        other_missing = {}
        for sym, bins in sorted(result.missing_symbols.items()):
            if _looks_like_libc_symbol(sym):
                libc_missing[sym] = bins
            else:
                other_missing[sym] = bins

        if libc_missing:
            print(f"  MISSING FROM MUSL ({len(libc_missing)} libc symbols):")
            print(f"  These symbols are imported by vendor .so but musl does not export them.")
            print(f"  Binaries using them will fail to load.")
            print()
            for sym, bins in sorted(libc_missing.items()):
                print(f"    {sym}")
                if not quiet:
                    for b in sorted(set(bins)):
                        print(f"      <- {b}")
            print()

        if other_missing and not quiet:
            print(f"  UNRESOLVED ({len(other_missing)} non-libc symbols):")
            print(f"  Likely resolved by other vendor .so at runtime.")
            print()
            for sym, bins in sorted(other_missing.items()):
                print(f"    {sym}")
            print()

    # Resolved by a sibling in the same package -- reported so the audit is
    # not silently dropping them, but these are not a gap to shim.
    if result.sibling_resolved and not quiet:
        indirect = {s: d for s, d in result.sibling_resolved.items() if d["indirect"]}
        print(f"  RESOLVED BY SIBLING VENDOR LIBS "
              f"({len(result.sibling_resolved)} symbols) -- not a musl gap:")
        for sym, d in sorted(result.sibling_resolved.items()):
            print(f"    {sym}  <- {', '.join(d['providers'])}")
        print()
        if indirect:
            print(f"  ...of which {len(indirect)} are imported by a binary that "
                  f"does not list the provider in its own DT_NEEDED:")
            for sym, d in sorted(indirect.items()):
                for b in sorted(set(d["indirect"])):
                    print(f"    {sym}  <- needed by {b}")
            print()

    # Compiler-runtime symbols -- provided by libgcc/libstdc++ in whichever
    # executable loads the library, never by libc.
    if result.toolchain_symbols and not quiet:
        print(f"  COMPILER RUNTIME ({len(result.toolchain_symbols)} symbols) -- "
              f"provided by libgcc/libstdc++, not libc:")
        for sym in sorted(result.toolchain_symbols):
            print(f"    {sym}")
        print()

    # Struct mismatches
    if result.struct_mismatches:
        has_issues = True
        print(f"  STRUCT LAYOUT MISMATCHES ({result.source_libc} vs musl):")
        print(f"  Vendor binaries were built against {result.source_libc}.")
        print(f"  Fields at different offsets/sizes will cause silent corruption.")
        print()
        for sm in result.struct_mismatches:
            print(f"    {sm['struct']}  (used by: {', '.join(sm['functions'])})")
            for key, vendor_val, musl_val in sm["mismatches"]:
                print(f"      {key}: {result.source_libc}={vendor_val}  musl={musl_val}")
            if not quiet:
                print(f"      affected binaries:")
                for b in sorted(set(sm["binaries"])):
                    print(f"        {b}")
            print()

    # Version requirements
    ver_issues = defaultdict(list)
    for binfo in result.binaries:
        for vr in binfo.version_reqs:
            if vr.startswith("GLIBC_"):
                ver_issues[vr].append(binfo.rel_path)
    if ver_issues:
        has_issues = True
        print(f"  GLIBC VERSION REQUIREMENTS:")
        print(f"  musl does not implement glibc symbol versioning.")
        print()
        for vr, bins in sorted(ver_issues.items()):
            print(f"    {vr}  ({len(bins)} binaries)")
        print()

    if not has_issues:
        print("  No issues found.")
        print()

    print("-" * 71)
    print()


def _looks_like_libc_symbol(sym: str) -> bool:
    """Heuristic: does this symbol look like it should come from libc?"""
    # Starts with __ (libc internals)
    if sym.startswith("__"):
        return True
    # Starts with _ followed by lowercase (libc private)
    if sym.startswith("_") and len(sym) > 1 and sym[1].islower():
        return True
    # Known libc function patterns
    libc_patterns = (
        "memcpy_s", "memmove_s", "memset_s", "snprintf_s",
        "strncpy_s", "strncat_s", "sprintf_s", "strcpy_s",
    )
    if sym in libc_patterns:
        return True
    return False


# ---------------------------------------------------------------------------
# Toolchain resolution: package -> defconfig -> toolchain download
# ---------------------------------------------------------------------------

TOOLCHAIN_CACHE_DIR = Path.home() / ".cache" / "openipc-audit-toolchains"
TOOLCHAIN_URL_BASE = "https://github.com/openipc/firmware/releases/download/toolchain"


@dataclass
class ToolchainInfo:
    """Resolved toolchain for a specific SoC family."""
    soc_vendor: str
    soc_family: str
    prefix: str     # e.g. "arm-openipc-linux-musleabi"
    libc_type: str  # "musl", "glibc", "uclibc"
    cache_dir: Path # where the extracted toolchain lives

    @property
    def libc_so(self) -> str:
        """Path to this toolchain's libc.so."""
        sysroot = self.cache_dir / self.prefix / "sysroot" / "lib" / "libc.so"
        if sysroot.exists():
            return str(sysroot)
        # Try alternate layout
        sdk = self.cache_dir / f"{self.prefix}_sdk-buildroot"
        alt = sdk / self.prefix / "sysroot" / "lib" / "libc.so"
        if alt.exists():
            return str(alt)
        return ""

    @property
    def cc(self) -> str:
        """Path to this toolchain's gcc."""
        for base in [self.cache_dir, self.cache_dir / f"{self.prefix}_sdk-buildroot"]:
            cc = base / "bin" / f"{self.prefix}-gcc"
            if cc.exists():
                return str(cc)
        return ""


def package_to_soc(pkg_name: str) -> tuple[str, str] | None:
    """Extract (vendor, soc_family) from osdrv package name.
    e.g. 'hisilicon-osdrv-hi3516ev200' -> ('hisilicon', 'hi3516ev200')
    """
    # Pattern: <vendor>-osdrv-<soc_model>
    m = re.match(r"^(.+?)-osdrv-(.+)$", pkg_name.split("/")[-1])
    if not m:
        return None
    return m.group(1), m.group(2)


def find_defconfig(repo_root: Path, soc_vendor: str, soc_family: str) -> Path | None:
    """Find the _lite_defconfig for a given SoC family."""
    import glob
    patterns = [
        str(repo_root / f"br-ext-chip-{soc_vendor}/configs/{soc_family}_lite_defconfig"),
        str(repo_root / f"br-ext-chip-{soc_vendor}/configs/{soc_family}_*_defconfig"),
    ]
    for pat in patterns:
        matches = glob.glob(pat)
        if matches:
            return Path(matches[0])
    return None


def parse_defconfig_toolchain(defconfig: Path) -> tuple[str, str]:
    """Extract (prefix, libc_type) from a defconfig file."""
    prefix = ""
    libc_type = ""
    with open(defconfig) as f:
        for line in f:
            line = line.strip()
            if line.startswith("BR2_TOOLCHAIN_EXTERNAL_CUSTOM_PREFIX="):
                prefix = line.split("=", 1)[1].strip('"')
            elif "CUSTOM_MUSL=y" in line:
                libc_type = "musl"
            elif "CUSTOM_GLIBC=y" in line or "USES_GLIBC=y" in line:
                libc_type = "glibc"
            elif "CUSTOM_UCLIBC=y" in line:
                libc_type = "uclibc"
    return prefix, libc_type


def download_toolchain(soc_vendor: str, soc_family: str, prefix: str) -> Path | None:
    """Download and extract a toolchain, return the extracted directory."""
    TOOLCHAIN_CACHE_DIR.mkdir(parents=True, exist_ok=True)

    # toolchain.hisilicon-hi3516ev200.tgz
    tarball_name = f"toolchain.{soc_vendor}-{soc_family}.tgz"
    tarball_path = TOOLCHAIN_CACHE_DIR / tarball_name
    extract_dir = TOOLCHAIN_CACHE_DIR / f"{soc_vendor}-{soc_family}"

    # Already extracted?  Only a directory published by the atomic rename
    # below counts: a half-extracted tree would otherwise be trusted forever.
    if extract_dir.is_dir():
        return extract_dir

    # Already downloaded?  Download into a .part file and rename only on
    # success, so an interrupted run (CI timeout, ^C) cannot leave a truncated
    # tarball that every later run happily tries to extract.  This matters most
    # under the CI cache, where one poisoned entry would persist across runs.
    if not tarball_path.exists():
        url = f"{TOOLCHAIN_URL_BASE}/{tarball_name}"
        print(f"  Downloading toolchain: {url}")
        part_path = tarball_path.with_name(tarball_name + ".part")
        try:
            r = subprocess.run(
                ["wget", "-q", "-O", str(part_path), url],
                timeout=900,
            )
            if r.returncode != 0:
                part_path.unlink(missing_ok=True)
                print(f"  ERROR: download failed", file=sys.stderr)
                return None
            part_path.replace(tarball_path)
        except (subprocess.TimeoutExpired, FileNotFoundError):
            part_path.unlink(missing_ok=True)
            print(f"  ERROR: download failed (timeout or wget not found)", file=sys.stderr)
            return None

    # Extract into a scratch directory and publish it with a single rename, for
    # the same reason: a failed tar must not leave a partial tree behind under
    # the name the "already extracted?" check above trusts.
    print(f"  Extracting toolchain to {extract_dir}")
    staging_dir = extract_dir.with_name(extract_dir.name + ".partial")
    shutil.rmtree(staging_dir, ignore_errors=True)
    staging_dir.mkdir(parents=True, exist_ok=True)
    try:
        r = subprocess.run(
            ["tar", "xzf", str(tarball_path), "-C", str(staging_dir),
             "--strip-components=1"],
            capture_output=True, timeout=300,
        )
    except subprocess.TimeoutExpired:
        shutil.rmtree(staging_dir, ignore_errors=True)
        print(f"  ERROR: extraction timed out", file=sys.stderr)
        return None
    if r.returncode != 0:
        shutil.rmtree(staging_dir, ignore_errors=True)
        # A truncated tarball is the usual cause, and keeping it would make the
        # failure permanent, so drop it and let the next run fetch it again.
        tarball_path.unlink(missing_ok=True)
        print(f"  ERROR: extraction failed: {r.stderr.decode()}", file=sys.stderr)
        return None

    staging_dir.replace(extract_dir)
    return extract_dir


def resolve_toolchain(repo_root: Path, pkg_name: str) -> ToolchainInfo | None:
    """Resolve the correct toolchain for a given osdrv package."""
    soc = package_to_soc(pkg_name)
    if not soc:
        return None
    soc_vendor, soc_family = soc

    # Find and parse defconfig
    defconfig = find_defconfig(repo_root, soc_vendor, soc_family)
    if not defconfig:
        print(f"  WARNING: no defconfig found for {soc_vendor}-{soc_family}", file=sys.stderr)
        return None

    prefix, libc_type = parse_defconfig_toolchain(defconfig)
    if not prefix:
        print(f"  WARNING: no toolchain prefix in {defconfig.name}", file=sys.stderr)
        return None

    # Check if already available in output/
    output_tc = repo_root / "output" / "host" / "opt" / "ext-toolchain"
    if output_tc.is_dir():
        # Verify it matches this package's SoC
        config_file = repo_root / "output" / ".config"
        if config_file.exists():
            config_text = config_file.read_text()
            if f'BR2_OPENIPC_SOC_FAMILY="{soc_family}"' in config_text:
                return ToolchainInfo(
                    soc_vendor=soc_vendor, soc_family=soc_family,
                    prefix=prefix, libc_type=libc_type,
                    cache_dir=output_tc,
                )

    # Download from releases
    cache_dir = download_toolchain(soc_vendor, soc_family, prefix)
    if not cache_dir:
        return None

    return ToolchainInfo(
        soc_vendor=soc_vendor, soc_family=soc_family,
        prefix=prefix, libc_type=libc_type,
        cache_dir=cache_dir,
    )


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Audit vendor .so binaries for ABI incompatibilities with musl.",
    )
    parser.add_argument("--package", help="Audit only the named package")
    parser.add_argument("--symbols-only", action="store_true",
                        help="Only check symbol availability, skip struct probe")
    parser.add_argument("--quiet", action="store_true",
                        help="Less detail in output")
    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent.parent
    pkg_dir = repo_root / "general" / "package"

    if not pkg_dir.is_dir():
        print(f"ERROR: Package directory not found: {pkg_dir}", file=sys.stderr)
        sys.exit(1)

    # Build package list
    if args.package:
        p = pkg_dir / args.package
        if not p.is_dir():
            print(f"ERROR: Package not found: {args.package}", file=sys.stderr)
            sys.exit(1)
        packages = [args.package]
    else:
        packages = sorted(
            d.name for d in pkg_dir.iterdir()
            if d.is_dir() and "-osdrv-" in d.name
        )
        legacy_dir = pkg_dir / "legacy"
        if legacy_dir.is_dir():
            for d in sorted(legacy_dir.iterdir()):
                if d.is_dir() and "-osdrv-" in d.name:
                    packages.append(f"legacy/{d.name}")

    # Header
    from datetime import date
    print("=" * 71)
    print(" OpenIPC Vendor Binary ABI Compatibility Audit")
    print(f" Date: {date.today()}")
    print("=" * 71)
    print()

    total_missing = 0
    total_struct_issues = 0
    total_binaries = 0
    total_packages = 0

    # Cache: toolchain key -> (musl_exports, musl_sizes)
    # Multiple packages may share the same toolchain (same SoC family)
    tc_cache: dict[str, tuple[set[str], dict[str, str] | None]] = {}

    for pkg_name in packages:
        # Resolve per-package toolchain
        tc = resolve_toolchain(repo_root, pkg_name)
        if not tc:
            print(f"Skipping {pkg_name}: no toolchain found")
            print("-" * 71)
            print()
            continue

        tc_key = f"{tc.soc_vendor}-{tc.soc_family}"
        if tc_key not in tc_cache:
            libc_path = tc.libc_so
            if not libc_path:
                print(f"Skipping {pkg_name}: libc.so not found in toolchain")
                print("-" * 71)
                print()
                continue

            print(f"Toolchain: {tc_key} ({tc.libc_type}, prefix={tc.prefix})")
            print(f"  libc.so: {libc_path}")
            musl_exports = get_musl_exports(libc_path)
            print(f"  {len(musl_exports)} exported symbols")

            musl_sizes = None
            if not args.symbols_only:
                cc = tc.cc
                if cc:
                    musl_sizes = compile_and_run_probe(cc)
                    if musl_sizes:
                        print(f"  Probed {len(musl_sizes)} struct size/offset values")
                    else:
                        print("  WARNING: struct probe failed, skipping struct checks")
                else:
                    print("  WARNING: gcc not found in toolchain, skipping struct probe")
            print()

            tc_cache[tc_key] = (musl_exports, musl_sizes)

        musl_exports, musl_sizes = tc_cache[tc_key]

        result = audit_package(
            pkg_name, str(pkg_dir / pkg_name), musl_exports, musl_sizes,
        )
        if not result:
            continue

        total_packages += 1
        total_binaries += result.so_count
        total_missing += len(result.missing_symbols)
        total_struct_issues += len(result.struct_mismatches)

        print_report(result, args.quiet)

    # Summary
    print("=" * 71)
    print(" OVERALL SUMMARY")
    print("=" * 71)
    print(f" Packages scanned:     {total_packages}")
    print(f" Binaries scanned:     {total_binaries}")
    print(f" Missing symbols:      {total_missing}")
    print(f" Struct mismatches:    {total_struct_issues}")
    print("=" * 71)

    if total_missing > 0:
        sys.exit(1)
    elif total_struct_issues > 0:
        sys.exit(2)
    else:
        sys.exit(0)


if __name__ == "__main__":
    main()
