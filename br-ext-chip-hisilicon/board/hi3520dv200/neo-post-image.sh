#!/bin/sh
# Post-image: append the DTB to zImage and wrap as uImage for hi3520dv200_neo.
# The SoC-wide DT, not a board's: this one image serves every Hi3520DV200, and
# the NBD6808T-PL's DT puts a framebuffer past the end of a 128 MB board's DDR
# and its NOR on CS1 only (OpenIPC/linux#201).
IMAGES_DIR="${BINARIES_DIR:-$1}"
BUILD_DIR="${BUILD_DIR:-$(dirname $IMAGES_DIR)/build}"
HOST_DIR="${HOST_DIR:-$(dirname $IMAGES_DIR)/host}"
KDIR="$BUILD_DIR/linux-custom"
ZIMAGE="$IMAGES_DIR/zImage"
DTB="$KDIR/arch/arm/boot/dts/hisilicon/hi3520dv200-generic.dtb"
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
[ -f "$ZIMAGE" ] || { echo "neo-post-image: ERROR: $ZIMAGE not found" >&2; exit 1; }
# The kernel comes from a rolling upstream-patches.tar.gz, and Buildroot keeps
# the first one it downloaded. The generic DT arrived with OpenIPC/linux#203.
[ -f "$DTB" ] || {
    echo "neo-post-image: ERROR: $DTB not found" >&2
    echo "neo-post-image: a cached upstream-patches.tar.gz older than OpenIPC/linux#203?" >&2
    echo "neo-post-image: delete it from the download dir, then make br-linux-dirclean" >&2
    exit 1
}

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
