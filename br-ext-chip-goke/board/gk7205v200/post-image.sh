#!/bin/bash
#
# gk7205v200-family NOR: firmware.bin = a hashed FIT (zImage + DTB, built from
# nor-fit.its) followed by the squashfs at the next 64 KiB boundary, padded to
# 64 KiB. Written as one image at 0x50000; the kernel splits that "firmware"
# partition into kernel, rootfs and rootfs_data at boot (mtdsplit), so only the
# whole has to fit the chip. The zImage and DTB come from the kernel tree:
# BINARIES_DIR only gets the uImage, with its DTB appended.
set -e

BINARIES_DIR=$1
BOARD_DIR="$(dirname "$0")"
ERASEBLOCK_SIZE=$((64 * 1024))

SOC=$(sed -n 's/^BR2_OPENIPC_SOC_MODEL="\(.*\)"/\1/p' "${BR2_CONFIG}")
KBOOT=$(ls -d "${BUILD_DIR}"/linux-*/arch/arm/boot 2>/dev/null | grep -v headers | head -1)
FIT_DIR="${BINARIES_DIR}/nor-fit"

rm -rf "${FIT_DIR}" && mkdir -p "${FIT_DIR}"
sed -e "s/@SOC@/${SOC}/" -e "s/@DTB@/${SOC}-demb.dtb/g" \
	"${BOARD_DIR}/nor-fit.its" > "${FIT_DIR}/nor-fit.its"
cp "${KBOOT}/zImage" "${FIT_DIR}/"
cp "${KBOOT}/dts/${SOC}-demb.dtb" "${FIT_DIR}/"
"${HOST_DIR}/bin/mkimage" -f "${FIT_DIR}/nor-fit.its" "${BINARIES_DIR}/fitImage"
rm -rf "${FIT_DIR}"

FIT_SIZE=$(stat -c%s "${BINARIES_DIR}/fitImage")
OFFSET_BLOCKS=$(( (FIT_SIZE + ERASEBLOCK_SIZE - 1) / ERASEBLOCK_SIZE ))
cp "${BINARIES_DIR}/fitImage" "${BINARIES_DIR}/firmware.bin"
dd if="${BINARIES_DIR}/rootfs.squashfs" of="${BINARIES_DIR}/firmware.bin" \
	bs=${ERASEBLOCK_SIZE} seek=${OFFSET_BLOCKS} conv=notrunc status=none
truncate -s %${ERASEBLOCK_SIZE} "${BINARIES_DIR}/firmware.bin"
echo "firmware.bin: FIT $((FIT_SIZE / 1024))K + squashfs $(( $(stat -c%s "${BINARIES_DIR}/rootfs.squashfs") / 1024 ))K = $(( $(stat -c%s "${BINARIES_DIR}/firmware.bin") / 1024 ))K"
