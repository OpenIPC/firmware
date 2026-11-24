#!/bin/sh
# Post-image: append the GM8126 DTB to zImage and wrap it as a uImage for gm8126_neo.
IMAGES_DIR="${BINARIES_DIR:-$1}"
BUILD_DIR="${BUILD_DIR:-$(dirname $IMAGES_DIR)/build}"
HOST_DIR="${HOST_DIR:-$(dirname $IMAGES_DIR)/host}"
KDIR="$BUILD_DIR/linux-custom"
ZIMAGE="$IMAGES_DIR/zImage"
DTB="$KDIR/arch/arm/boot/dts/grainmedia/gm8126-ipcam.dtb"
LOADADDR=0x00008000

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

KREL=$(sed 's/-//g' "$KDIR/include/config/kernel.release")

cat "$ZIMAGE" "$DTB" > "${IMAGES_DIR}/zImage-dtb"
$MKIMAGE -A arm -O linux -T kernel -C none \
    -a $LOADADDR -e $LOADADDR \
    -n "Linux-${KREL}-gm8126" \
    -d "${IMAGES_DIR}/zImage-dtb" "${IMAGES_DIR}/uImage"
rm -f "${IMAGES_DIR}/zImage-dtb"
if [ -f "${IMAGES_DIR}/uImage" ]; then
    echo "neo-post-image: uImage $(( $(stat -c%s ${IMAGES_DIR}/uImage) / 1024 ))KB"
else
    echo "neo-post-image: ERROR: mkimage failed" >&2
    exit 1
fi
