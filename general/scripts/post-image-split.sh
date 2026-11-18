#!/bin/bash
#
# NOR split layout: firmware.bin = a legacy uImage, padded to 64 KiB, then the
# squashfs. Written as one image where the board's "firmware" partition starts
# (0x50000, or after a larger boot area), where every U-Boot -- a vendor's
# included -- loads and bootm's the kernel. The kernel finds the squashfs right
# after itself and divides "firmware" into kernel, rootfs and rootfs_data at
# boot (mtdsplit), so only the whole has to fit the chip.
#
# The uImage is the kernel build's own (32-bit ARM: the zImage with its DTB
# appended). An arm64 kernel cannot carry an appended DTB, so there it is a
# legacy multi-file uImage instead -- Image.gz, no ramdisk, the DTB -- which
# bootm unpacks the same way; the header, and so the split, is the same.
set -e

BINARIES_DIR=$1
ERASEBLOCK_SIZE=$((64 * 1024))

if [ -f "${BINARIES_DIR}/Image" ]; then
	# Buildroot drops the load address from its .config unless it builds a
	# uImage itself; the board's composed defconfig, beside images/, has it.
	loadaddr=$(sed -n 's/^BR2_LINUX_KERNEL_UIMAGE_LOADADDR="\(.*\)"$/\1/p' \
		"${BR2_CONFIG}" "${BINARIES_DIR}/../openipc_defconfig" 2>/dev/null | head -n 1)
	dtbs=$(ls "${BINARIES_DIR}"/*.dtb)
	if [ -z "${loadaddr}" ] || [ "$(echo "${dtbs}" | wc -w)" != 1 ]; then
		echo "post-image-split: an arm64 uImage needs BR2_LINUX_KERNEL_UIMAGE_LOADADDR and one DTB" >&2
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

KERNEL="${BINARIES_DIR}/uImage"
KERNEL_SIZE=$(stat -c%s "${KERNEL}")
OFFSET_BLOCKS=$(( (KERNEL_SIZE + ERASEBLOCK_SIZE - 1) / ERASEBLOCK_SIZE ))
cp "${KERNEL}" "${BINARIES_DIR}/firmware.bin"
dd if="${BINARIES_DIR}/rootfs.squashfs" of="${BINARIES_DIR}/firmware.bin" \
	bs=${ERASEBLOCK_SIZE} seek=${OFFSET_BLOCKS} conv=notrunc status=none
truncate -s %${ERASEBLOCK_SIZE} "${BINARIES_DIR}/firmware.bin"
echo "firmware.bin: uImage $((KERNEL_SIZE / 1024))K + squashfs $(( $(stat -c%s "${BINARIES_DIR}/rootfs.squashfs") / 1024 ))K = $(( $(stat -c%s "${BINARIES_DIR}/firmware.bin") / 1024 ))K"
