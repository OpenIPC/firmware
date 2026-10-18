#!/bin/bash
#
# gk7205v200-family NOR: firmware.bin = the uImage (zImage with its DTB
# appended, as the fixed-slot package carries it), padded to 64 KiB, then the
# squashfs. Written as one image at 0x50000, where every U-Boot -- a vendor's
# included -- loads and bootm's the kernel; the kernel finds the squashfs right
# after itself and divides the "firmware" partition into kernel, rootfs and
# rootfs_data at boot (mtdsplit), so only the whole has to fit the chip.
set -e

BINARIES_DIR=$1
ERASEBLOCK_SIZE=$((64 * 1024))

KERNEL="${BINARIES_DIR}/uImage"
KERNEL_SIZE=$(stat -c%s "${KERNEL}")
OFFSET_BLOCKS=$(( (KERNEL_SIZE + ERASEBLOCK_SIZE - 1) / ERASEBLOCK_SIZE ))
cp "${KERNEL}" "${BINARIES_DIR}/firmware.bin"
dd if="${BINARIES_DIR}/rootfs.squashfs" of="${BINARIES_DIR}/firmware.bin" \
	bs=${ERASEBLOCK_SIZE} seek=${OFFSET_BLOCKS} conv=notrunc status=none
truncate -s %${ERASEBLOCK_SIZE} "${BINARIES_DIR}/firmware.bin"
echo "firmware.bin: uImage $((KERNEL_SIZE / 1024))K + squashfs $(( $(stat -c%s "${BINARIES_DIR}/rootfs.squashfs") / 1024 ))K = $(( $(stat -c%s "${BINARIES_DIR}/firmware.bin") / 1024 ))K"
