#!/bin/bash

BINARIES_DIR=$1
BOARD_DIR="$(dirname "$0")"
ITS_SOURCE="fit-image.its"

ERASEBLOCK_SIZE=$((64 * 1024))


cp ${BINARIES_DIR}/hi35*.dtb ${BINARIES_DIR}/fdt.dtb
# Compress the arm64 Image so the FIT + squashfs fit the 16 MiB NOR. U-Boot
# decompresses it at boot (CONFIG_FIT + CONFIG_GZIP).
gzip -9 -c ${BINARIES_DIR}/Image > ${BINARIES_DIR}/Image.gz
cp ${BOARD_DIR}/${ITS_SOURCE} ${BINARIES_DIR}/
mkimage -f ${BINARIES_DIR}/${ITS_SOURCE} ${BINARIES_DIR}/fitImage

FIT_SIZE=$(stat -c%s "${BINARIES_DIR}/fitImage")
OFFSET_BLOCKS=$(( (FIT_SIZE + ERASEBLOCK_SIZE - 1) / ERASEBLOCK_SIZE ))

cp ${BINARIES_DIR}/fitImage ${BINARIES_DIR}/firmware.bin

dd if=${BINARIES_DIR}/rootfs.squashfs of=${BINARIES_DIR}/firmware.bin \
   bs=${ERASEBLOCK_SIZE} seek=${OFFSET_BLOCKS} conv=notrunc

truncate -s %${ERASEBLOCK_SIZE} ${BINARIES_DIR}/firmware.bin
