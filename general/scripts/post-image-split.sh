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

# The image is only bootable by a kernel that divides it: mtdsplit
# (MTD_SPLIT_FIRMWARE), OpenIPC's split layout (MTD_SPLIT_LAYOUT), or the
# OpenIPC uImage DT partition parser (MTD_PARSER_OPENIPC_UIMAGE). Kconfig
# drops a symbol its tree does not know, so a kernel that predates all three
# builds without a word and cannot find its rootfs -- refuse the image instead.
if ! grep -qsE '^CONFIG_MTD_(SPLIT_(FIRMWARE|LAYOUT)|PARSER_OPENIPC_UIMAGE)=y$' "${BUILD_DIR}"/linux-*/.config; then
	echo "post-image-split: the kernel (${BUILD_DIR}/linux-*/.config) has none of" \
		"CONFIG_MTD_SPLIT_FIRMWARE, CONFIG_MTD_SPLIT_LAYOUT, or" \
		"CONFIG_MTD_PARSER_OPENIPC_UIMAGE: it cannot boot a split firmware.bin" >&2
	exit 1
fi

# arm64: compose the uImage (Image + DTB) first, as the 32-bit kernel build
# does itself.
"$(dirname "$0")/post-image-arm64-uimage.sh" "${BINARIES_DIR}"

KERNEL="${BINARIES_DIR}/uImage"
KERNEL_SIZE=$(stat -c%s "${KERNEL}")
OFFSET_BLOCKS=$(( (KERNEL_SIZE + ERASEBLOCK_SIZE - 1) / ERASEBLOCK_SIZE ))
cp "${KERNEL}" "${BINARIES_DIR}/firmware.bin"
dd if="${BINARIES_DIR}/rootfs.squashfs" of="${BINARIES_DIR}/firmware.bin" \
	bs=${ERASEBLOCK_SIZE} seek=${OFFSET_BLOCKS} conv=notrunc status=none
truncate -s %${ERASEBLOCK_SIZE} "${BINARIES_DIR}/firmware.bin"
echo "firmware.bin: uImage $((KERNEL_SIZE / 1024))K + squashfs $(( $(stat -c%s "${BINARIES_DIR}/rootfs.squashfs") / 1024 ))K = $(( $(stat -c%s "${BINARIES_DIR}/firmware.bin") / 1024 ))K"
