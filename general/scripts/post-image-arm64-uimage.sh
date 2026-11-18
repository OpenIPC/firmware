#!/bin/bash
#
# arm64 uImage: Buildroot builds no uImage for arm64, so this composes one from
# the kernel's Image and its single DTB -- a legacy multi-file uImage the
# vendors' U-Boots bootm. A no-op when the build has no arm64 Image.
#
# Runs as the post-image script of an arm64 board on fixed NOR slots
# (ax620e_lite), and from post-image-split.sh for the split layout.
set -e

BINARIES_DIR=$1

if [ -f "${BINARIES_DIR}/Image" ]; then
	# Buildroot drops the load address from its .config unless it builds a
	# uImage itself; the board's composed defconfig, beside images/, has it.
	loadaddr=$(sed -n 's/^BR2_LINUX_KERNEL_UIMAGE_LOADADDR="\(.*\)"$/\1/p' \
		"${BR2_CONFIG}" "${BINARIES_DIR}/../openipc_defconfig" 2>/dev/null | head -n 1)
	dtbs=$(ls "${BINARIES_DIR}"/*.dtb)
	if [ -z "${loadaddr}" ] || [ "$(echo "${dtbs}" | wc -w)" != 1 ]; then
		echo "post-image-arm64-uimage: an arm64 uImage needs BR2_LINUX_KERNEL_UIMAGE_LOADADDR and one DTB" >&2
		exit 1
	fi
	# gzip unless the board says otherwise: the Axera/Xiongmai U-Boot has no
	# gunzip in bootm ("Unimplemented compression type 1"), only LZMA.
	comp=$(sed -n 's/^BR2_OPENIPC_ARM64_UIMAGE_COMP="\(.*\)"$/\1/p' \
		"${BR2_CONFIG}" "${BINARIES_DIR}/../openipc_defconfig" 2>/dev/null | head -n 1)
	if [ "${comp}" = "lzma" ]; then
		lzma -9 -c "${BINARIES_DIR}/Image" > "${BINARIES_DIR}/Image.gz"
		comptype=3
	else
		gzip -9 -n -c "${BINARIES_DIR}/Image" > "${BINARIES_DIR}/Image.gz"
		comptype=1
	fi
	# Linux, arm64, multi-file, gzip: Image.gz, a ramdisk, and the DTB, which
	# bootm hands the kernel from the third component. A multi-file image's
	# size table ends at the first zero, so the ramdisk cannot be empty: it is
	# a cpio archive holding only its trailer, an empty initramfs, after which
	# the kernel mounts its root from the flash as it would without one.
	# Named as the kernel build names its own uImages, "Linux-<release>-<soc>",
	# which is what sysupgrade checks against the camera.
	release=$(cat "${BUILD_DIR}"/linux-*/include/config/kernel.release 2>/dev/null | head -n 1)
	python3 - "${BINARIES_DIR}/uImage" "${loadaddr}" "Linux-${release}-${OPENIPC_SOC_MODEL}" \
		"${BINARIES_DIR}/Image.gz" "${dtbs}" "${comptype}" <<'PY'
import struct, sys, time, zlib
out, load, name, kernel, dtb, comp = sys.argv[1:]
trailer = b'TRAILER!!!\0'
cpio = b'070701' + b''.join(b'%08X' % v for v in
                            (0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, len(trailer), 0)) + trailer
cpio += b'\0' * (-len(cpio) % 4)
kernel = open(kernel, 'rb').read()
# arm64 Linux refuses an FDT that is not 8-byte aligned ("invalid device
# tree blob", then a silent spin in setup_arch), and a U-Boot with
# fdt_high=0xffffffffffffffff -- the Axera/Xiongmai one -- boots it in place
# inside this image. The multi-file format only pads parts to 4 bytes, so
# grow the empty cpio by 4 zero bytes when the DTB would land on a 4.
if (64 + 16 + len(kernel) + (-len(kernel) % 4) + len(cpio)) % 8:
    cpio += b'\0' * 4
parts = [kernel, cpio, open(dtb, 'rb').read()]
data = struct.pack('>%dI' % (len(parts) + 1), *[len(p) for p in parts], 0)
for p in parts:
    data += p + b'\0' * (-len(p) % 4)
load = int(load, 0)
hdr = struct.pack('>7I4B32s', 0x27051956, 0, int(time.time()), len(data), load, load,
                  zlib.crc32(data) & 0xffffffff, 5, 22, 4, int(comp), name.encode()[:31])
hdr = hdr[:4] + struct.pack('>I', zlib.crc32(hdr) & 0xffffffff) + hdr[8:]
open(out, 'wb').write(hdr + data)
PY
fi
