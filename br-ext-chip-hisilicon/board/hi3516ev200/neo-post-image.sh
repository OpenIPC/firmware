#!/bin/sh
# Post-image: append DTB to zImage and wrap as uImage for hi3516ev300_neo.
# Needed because 7.0+ kernel can't build uImage via buildroot's old mkimage.
IMAGES_DIR="${BINARIES_DIR:-$1}"
BUILD_DIR="${BUILD_DIR:-$(dirname $IMAGES_DIR)/build}"
HOST_DIR="${HOST_DIR:-$(dirname $IMAGES_DIR)/host}"
KDIR="$BUILD_DIR/linux-custom"
ZIMAGE="$IMAGES_DIR/zImage"
DTB="$KDIR/arch/arm/boot/dts/hisilicon/hi3516ev300-demb.dtb"
LOADADDR=0x40008000
# Find mkimage: try buildroot host dir, per-package dir, then system PATH
MKIMAGE=""
for p in \
    "${HOST_DIR}/bin/mkimage" \
    "$(dirname $IMAGES_DIR)/host/bin/mkimage" \
    "$(find $(dirname $IMAGES_DIR)/per-package -name mkimage -type f 2>/dev/null | head -1)" \
    "$(which mkimage 2>/dev/null)"; do
    [ -x "$p" ] && MKIMAGE="$p" && break
done
[ -z "$MKIMAGE" ] && echo "neo-post-image: ERROR: mkimage not found" >&2 && exit 1

[ -f "$ZIMAGE" ] || exit 0
[ -f "$DTB" ] || exit 0

# Remove dashes from version so sysupgrade SoC parsing works
KREL=$(sed 's/-//g' "$KDIR/include/config/kernel.release")

cat "$ZIMAGE" "$DTB" > "${IMAGES_DIR}/zImage-dtb"
$MKIMAGE -A arm -O linux -T kernel -C none \
    -a $LOADADDR -e $LOADADDR \
    -n "Linux-${KREL}-hi3516ev300" \
    -d "${IMAGES_DIR}/zImage-dtb" "${IMAGES_DIR}/uImage"
rm -f "${IMAGES_DIR}/zImage-dtb"
if [ -f "${IMAGES_DIR}/uImage" ]; then
    echo "neo-post-image: uImage $(( $(stat -c%s ${IMAGES_DIR}/uImage) / 1024 ))KB"
else
    echo "neo-post-image: ERROR: mkimage failed" >&2
    exit 1
fi
