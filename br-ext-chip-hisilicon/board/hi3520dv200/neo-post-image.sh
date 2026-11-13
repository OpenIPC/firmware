#!/bin/sh
# Post-image: append the DTB to zImage and wrap as uImage for hi3520dv200_neo.
IMAGES_DIR="${BINARIES_DIR:-$1}"
BUILD_DIR="${BUILD_DIR:-$(dirname $IMAGES_DIR)/build}"
HOST_DIR="${HOST_DIR:-$(dirname $IMAGES_DIR)/host}"
KDIR="$BUILD_DIR/linux-custom"
ZIMAGE="$IMAGES_DIR/zImage"
DTB="$KDIR/arch/arm/boot/dts/hisilicon/hi3520dv200-nbd6808t.dtb"
LOADADDR=0x80008000

MKIMAGE=""
for p in \
    "${HOST_DIR}/bin/mkimage" \
    "$(dirname $IMAGES_DIR)/host/bin/mkimage" \
    "$(find $(dirname $IMAGES_DIR)/per-package -name mkimage -type f 2>/dev/null | head -1)" \
    "$(which mkimage 2>/dev/null)"; do
    [ -x "$p" ] && MKIMAGE="$p" && break
done
[ -z "$MKIMAGE" ] && echo "neo-post-image: ERROR: mkimage not found" >&2 && exit 1

# A missing input is a broken build, not a skip: the NOR packaging step would
# otherwise pick up whatever uImage an earlier build left behind.
for f in "$ZIMAGE" "$DTB"; do
    [ -f "$f" ] || { echo "neo-post-image: ERROR: $f not found" >&2; exit 1; }
done

KREL=$(sed 's/-//g' "$KDIR/include/config/kernel.release")

rm -f "${IMAGES_DIR}/uImage"
cat "$ZIMAGE" "$DTB" > "${IMAGES_DIR}/zImage-dtb" &&
$MKIMAGE -A arm -O linux -T kernel -C none \
    -a $LOADADDR -e $LOADADDR \
    -n "Linux-${KREL}-hi3520dv200" \
    -d "${IMAGES_DIR}/zImage-dtb" "${IMAGES_DIR}/uImage"
rc=$?
rm -f "${IMAGES_DIR}/zImage-dtb"
if [ $rc -ne 0 ] || [ ! -f "${IMAGES_DIR}/uImage" ]; then
    echo "neo-post-image: ERROR: mkimage failed" >&2
    exit 1
fi
echo "neo-post-image: uImage $(( $(stat -c%s ${IMAGES_DIR}/uImage) / 1024 ))KB"
