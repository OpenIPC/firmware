#!/usr/bin/env python3
"""
Per-package + per-kernel-module size report for a built OpenIPC firmware tree.

Reads environment variables exported by the top-level Makefile's `size-report`
target, walks the assembled target dir (the post-finalize-hook truth), attributes
every inode to a Buildroot package via packages-file-list.txt, special-cases the
/lib/modules tree, reports squashfs compression + headroom, and writes a single
schema-versioned JSON into IMAGES_DIR.

Required env:
  TARGET_DIR              final assembled rootfs
  BR2_OUTPUT_DIR          Buildroot output dir (has build/, per-package/, etc.)
  IMAGES_DIR              dir holding uImage / rootfs.squashfs and where to write
  OPENIPC_SOC_MODEL       e.g. hi3518ev300
  OPENIPC_VARIANT         lite | ultimate | neo
  BR2_OPENIPC_FLASH_SIZE  "8" | "16"

Optional env:
  BR2_TARGET_ROOTFS_SQUASHFS=y / BR2_TARGET_ROOTFS_UBI=y  (used for cap lookup)
  BR2_OPENIPC_SOC_VENDOR  used for rockchip cap exception
  BR2_OVERLAY_PATH        defaults to BR2_OUTPUT_DIR/../general/overlay

Run after `make BOARD=...` and before or after `repack` (handles both raw and
`.<soc_model>`-suffixed image filenames).
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

SCHEMA = 1
TOP_FILES_PER_PKG = 5
OVERLAY_BUCKET = "_overlay_or_post_build"

# Path patterns that Buildroot's default target-finalize strip pass removes
# (staging artefacts that packages-file-list.txt records but never reach the
# camera). Filtering these from `removed_by_finalize` leaves only OpenIPC-
# attributable removals — TRIM_SP2308, FINALIZE_MODULES open_*.ko cleanup,
# rootfs_script.sh's libstdc++ strip, and the per-board excludes list.
BR_STRIP_SUFFIXES = (".a", ".la", ".h", ".hh", ".hpp", ".hxx", ".pc", ".cmake")
BR_STRIP_PREFIXES = (
    "usr/include/",
    "usr/lib/pkgconfig/",
    "usr/lib/cmake/",
    "usr/share/man/",
    "usr/share/doc/",
    "usr/share/info/",
    "usr/share/aclocal/",
    "usr/share/cmake/",
    "usr/share/gtk-doc/",
    "usr/share/locale/",
)


def is_buildroot_strip(rel: str) -> bool:
    """True if `rel` looks like something Buildroot's default finalize removes."""
    if rel.endswith(BR_STRIP_SUFFIXES):
        return True
    return rel.startswith(BR_STRIP_PREFIXES)


def env(name: str, *, required: bool = True, default: str | None = None) -> str:
    val = os.environ.get(name, default)
    if required and not val:
        sys.exit(f"size_report: missing required env var {name}")
    return val or ""


def find_image(images_dir: Path, base: str, soc_model: str) -> Path | None:
    """REPACK_FIRMWARE renames foo -> foo.<soc_model>. Accept either form."""
    for name in (base, f"{base}.{soc_model}"):
        p = images_dir / name
        if p.exists():
            return p
    return None


def parse_packages_file_list(path: Path) -> dict[str, str]:
    """Map relative_path -> package. Last writer wins (matches Buildroot)."""
    mapping: dict[str, str] = {}
    with path.open() as f:
        for line in f:
            line = line.rstrip("\n")
            if not line or "," not in line:
                continue
            pkg, rel = line.split(",", 1)
            if rel.startswith("./"):
                rel = rel[2:]
            mapping[rel] = pkg
    return mapping


def walk_target(target_dir: Path) -> tuple[list[tuple[str, int]], int]:
    """
    Return (entries, total_bytes). entries is a list of (rel_path, size).
    Hardlink-dedup: charge each inode exactly once, to its first-seen path.
    Symlinks: charge the symlink's own size (lstat), don't follow.
    Special files (sockets/fifos/devices): contribute 0 bytes but still listed.
    """
    seen_inodes: set[tuple[int, int]] = set()
    entries: list[tuple[str, int]] = []
    total = 0
    target_dir = target_dir.resolve()
    for dirpath, _dirnames, filenames in os.walk(target_dir, followlinks=False):
        for name in filenames:
            full = Path(dirpath) / name
            try:
                st = full.lstat()
            except OSError:
                continue
            inode_key = (st.st_dev, st.st_ino)
            if not full.is_symlink() and st.st_nlink > 1:
                if inode_key in seen_inodes:
                    continue
                seen_inodes.add(inode_key)
            rel = str(full.relative_to(target_dir))
            entries.append((rel, st.st_size))
            total += st.st_size
    return entries, total


def squashfs_metadata(path: Path) -> dict | None:
    """Run `unsquashfs -s` and parse out compression + on-disk byte total."""
    try:
        out = subprocess.check_output(
            ["unsquashfs", "-s", str(path)], text=True, stderr=subprocess.STDOUT
        )
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        return {"error": str(e)}

    info: dict = {"compressed_bytes": path.stat().st_size}
    for line in out.splitlines():
        m = re.match(r"\s*Compression\s+(\S+)", line)
        if m:
            info["compression"] = m.group(1)
        m = re.match(r"\s*Block size\s+(\d+)", line)
        if m:
            info["block_size"] = int(m.group(1))
        m = re.match(r"\s*Number of inodes\s+(\d+)", line)
        if m:
            info["inode_count"] = int(m.group(1))
    return info


def kernel_caps(flash_mb: str, vendor: str, has_ubi: bool) -> dict[str, int]:
    """Mirror the size limits the Makefile's CHECK_SIZE enforces."""
    vendor = vendor.strip('"')
    if has_ubi:
        if vendor in ("rockchip", "sigmastar"):
            return {"kernel_kb": 0, "rootfs_kb": 16384}
        return {"kernel_kb": 4096, "rootfs_kb": 16384}
    if vendor == "rockchip":
        return {"kernel_kb": 4096, "rootfs_kb": 8192}
    if flash_mb.strip('"') == "8":
        return {"kernel_kb": 2048, "rootfs_kb": 5120}
    return {"kernel_kb": 2048, "rootfs_kb": 8192}


def parse_autoload(overlay_dir: Path) -> set[str]:
    """Modules listed in /etc/modules (auto-loaded by S35modules at boot)."""
    f = overlay_dir / "etc" / "modules"
    if not f.exists():
        return set()
    names: set[str] = set()
    for line in f.read_text().splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        names.add(line.split()[0])
    return names


def parse_builtin(target_dir: Path) -> tuple[str, list[str]]:
    """Find modules.builtin and return (kernel_version, list of builtin names)."""
    mods_root = target_dir / "lib" / "modules"
    if not mods_root.is_dir():
        return ("", [])
    versions = [p.name for p in mods_root.iterdir() if p.is_dir()]
    if not versions:
        return ("", [])
    kver = sorted(versions)[0]
    builtin = mods_root / kver / "modules.builtin"
    names: list[str] = []
    if builtin.exists():
        for line in builtin.read_text().splitlines():
            line = line.strip()
            if line.endswith(".ko"):
                names.append(Path(line).stem)
    return (kver, names)


def find_vmlinux(br_output: Path) -> Path | None:
    for candidate in br_output.glob("build/linux-*/vmlinux"):
        if candidate.is_file():
            return candidate
    return None


def build_report() -> dict:
    target_dir = Path(env("TARGET_DIR"))
    br_output = Path(env("BR2_OUTPUT_DIR"))
    images_dir = Path(env("IMAGES_DIR"))
    soc_model = env("OPENIPC_SOC_MODEL")
    variant = env("OPENIPC_VARIANT")
    flash_mb = env("BR2_OPENIPC_FLASH_SIZE", required=False, default="")
    vendor = env("BR2_OPENIPC_SOC_VENDOR", required=False, default="")
    overlay_dir = Path(
        env(
            "BR2_OVERLAY_PATH",
            required=False,
            default=str(br_output.parent / "general" / "overlay"),
        )
    )

    pfl = br_output / "build" / "packages-file-list.txt"
    if not pfl.exists():
        sys.exit(f"size_report: packages-file-list.txt not found at {pfl}")

    path_to_pkg = parse_packages_file_list(pfl)
    entries, rootfs_uncompressed = walk_target(target_dir)

    per_pkg_bytes: dict[str, int] = defaultdict(int)
    per_pkg_count: dict[str, int] = defaultdict(int)
    per_pkg_top: dict[str, list[tuple[int, str]]] = defaultdict(list)
    on_disk_paths: set[str] = set()

    for rel, size in entries:
        on_disk_paths.add(rel)
        pkg = path_to_pkg.get(rel, OVERLAY_BUCKET)
        per_pkg_bytes[pkg] += size
        per_pkg_count[pkg] += 1
        per_pkg_top[pkg].append((size, rel))

    removed: list[dict] = []
    for rel, pkg in path_to_pkg.items():
        if rel in on_disk_paths:
            continue
        if is_buildroot_strip(rel):
            continue
        src_bytes = 0
        per_pkg_target = br_output / "per-package" / pkg / "target" / rel
        if per_pkg_target.exists():
            try:
                src_bytes = per_pkg_target.lstat().st_size
            except OSError:
                pass
        removed.append({"path": "/" + rel, "package": pkg, "source_bytes": src_bytes})

    has_squashfs = (
        env("BR2_TARGET_ROOTFS_SQUASHFS", required=False, default="") == "y"
    )
    has_ubi = env("BR2_TARGET_ROOTFS_UBI", required=False, default="") == "y"

    rootfs_img = (
        find_image(images_dir, "rootfs.squashfs", soc_model) if has_squashfs else None
    )
    sq_meta = squashfs_metadata(rootfs_img) if rootfs_img else None

    rootfs_compressed = (
        sq_meta.get("compressed_bytes") if sq_meta and "error" not in sq_meta else None
    )
    ratio = (
        rootfs_compressed / rootfs_uncompressed
        if rootfs_compressed and rootfs_uncompressed
        else None
    )

    packages = []
    for pkg, total in sorted(per_pkg_bytes.items(), key=lambda kv: -kv[1]):
        top = sorted(per_pkg_top[pkg], reverse=True)[:TOP_FILES_PER_PKG]
        packages.append(
            {
                "name": pkg,
                "uncompressed_bytes": total,
                "compressed_bytes_approx": (
                    int(total * ratio) if ratio is not None else None
                ),
                "file_count": per_pkg_count[pkg],
                "top_files": [{"path": "/" + p, "bytes": b} for b, p in top],
            }
        )

    kver, builtin_names = parse_builtin(target_dir)
    autoload = parse_autoload(overlay_dir)

    modules: list[dict] = []
    mod_root = target_dir / "lib" / "modules" / kver if kver else None
    if mod_root and mod_root.is_dir():
        for full in sorted(mod_root.rglob("*.ko")):
            rel = str(full.relative_to(target_dir))
            name = full.stem
            try:
                size = full.lstat().st_size
            except OSError:
                size = 0
            modules.append(
                {
                    "name": name,
                    "path": "/" + rel,
                    "bytes": size,
                    "package": path_to_pkg.get(rel, OVERLAY_BUCKET),
                    "autoloaded": name in autoload,
                }
            )

    uimage = find_image(images_dir, "uImage", soc_model)
    zboot = find_image(images_dir, "zboot.img", soc_model)
    kernel_img = uimage or zboot
    vmlinux = find_vmlinux(br_output)

    kernel_block = {
        "image_path": str(kernel_img.relative_to(images_dir)) if kernel_img else None,
        "uimage_bytes": kernel_img.stat().st_size if kernel_img else None,
        "vmlinux_bytes": vmlinux.stat().st_size if vmlinux else None,
    }

    caps = kernel_caps(flash_mb, vendor, has_ubi)
    kernel_used_kb = (
        (kernel_img.stat().st_size + 1023) // 1024 if kernel_img else 0
    )
    rootfs_used_kb = (
        (rootfs_compressed + 1023) // 1024 if rootfs_compressed else 0
    )

    headroom = {
        "kernel": {
            "used_kb": kernel_used_kb,
            "cap_kb": caps["kernel_kb"],
            "headroom_kb": caps["kernel_kb"] - kernel_used_kb if caps["kernel_kb"] else None,
        },
        "rootfs": {
            "used_kb": rootfs_used_kb,
            "cap_kb": caps["rootfs_kb"],
            "headroom_kb": caps["rootfs_kb"] - rootfs_used_kb if caps["rootfs_kb"] else None,
        },
    }

    return {
        "schema": SCHEMA,
        "board": soc_model,
        "variant": variant,
        "flash_mb": int(flash_mb.strip('"')) if flash_mb.strip('"').isdigit() else None,
        "kernel_version": kver or None,
        "rootfs": {
            "uncompressed_bytes": rootfs_uncompressed,
            "compressed_bytes": rootfs_compressed,
            "compression": (sq_meta or {}).get("compression"),
            "compression_ratio": ratio,
        },
        "kernel": kernel_block,
        "headroom": headroom,
        "packages": packages,
        "linux_components": {
            "kernel_image": kernel_block,
            "modules": modules,
            "built_in": sorted(builtin_names),
            "autoload_list": sorted(autoload),
        },
        "removed_by_finalize": sorted(
            removed, key=lambda r: -r["source_bytes"]
        ),
    }


def main() -> None:
    report = build_report()
    images_dir = Path(env("IMAGES_DIR"))
    out_path = images_dir / f"sizes.{report['board']}-{report['variant']}.json"
    images_dir.mkdir(parents=True, exist_ok=True)
    out_path.write_text(json.dumps(report, indent=2) + "\n")
    pkgs = len(report["packages"])
    mods = len(report["linux_components"]["modules"])
    rem = len(report["removed_by_finalize"])
    rootfs = report["rootfs"]
    headroom = report["headroom"]
    print(
        f"size-report: {out_path}\n"
        f"  packages={pkgs} modules={mods} removed={rem}\n"
        f"  rootfs={rootfs.get('compressed_bytes')}B "
        f"(uncompressed {rootfs.get('uncompressed_bytes')}B, "
        f"ratio {rootfs.get('compression_ratio')})\n"
        f"  headroom: kernel={headroom['kernel']['headroom_kb']}KB "
        f"rootfs={headroom['rootfs']['headroom_kb']}KB"
    )


if __name__ == "__main__":
    main()
