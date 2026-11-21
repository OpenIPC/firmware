#!/bin/sh
# Post-image: append the T23 DTB to vmlinux.bin, LZMA-compress, and wrap as a
# legacy uImage before post-image-split.sh packs firmware.bin.
# Stock T23 U-Boot loads uImage at 0x80600000 and decompresses to 0x80100000,
# so a payload >= 4.5 MiB (4718592 bytes) would corrupt itself at boot.
set -e

IMAGES_DIR="${BINARIES_DIR:-$1}"
[ -n "$IMAGES_DIR" ] || {
	echo "v4l2-post-image: ERROR: BINARIES_DIR not set" >&2
	exit 1
}
BASE_DIR="$(dirname "$IMAGES_DIR")"
BUILD_DIR="${BUILD_DIR:-${BASE_DIR}/build}"
HOST_DIR="${HOST_DIR:-${BASE_DIR}/host}"
KDIR="${BUILD_DIR}/linux-custom"

find_tool() {
	for name in "$@"; do
		for p in "${HOST_DIR}/bin/${name}" "${BASE_DIR}/host/bin/${name}"; do
			if [ -x "$p" ]; then
				printf '%s\n' "$p"
				return 0
			fi
		done
		if [ -d "${BASE_DIR}/per-package" ]; then
			p=$(find "${BASE_DIR}/per-package" -name "${name}" -type f 2>/dev/null | head -n 1 || true)
			if [ -n "$p" ] && [ -x "$p" ]; then
				printf '%s\n' "$p"
				return 0
			fi
		fi
		p=$(command -v "${name}" 2>/dev/null || true)
		if [ -n "$p" ] && [ -x "$p" ]; then
			printf '%s\n' "$p"
			return 0
		fi
	done
	return 1
}

MKIMAGE=$(find_tool mkimage) || {
	echo "v4l2-post-image: ERROR: mkimage not found" >&2
	exit 1
}
XZ=$(find_tool xz) || {
	echo "v4l2-post-image: ERROR: xz not found" >&2
	exit 1
}
READELF=$(find_tool mipsel-openipc-linux-musl-readelf readelf) || {
	echo "v4l2-post-image: ERROR: readelf not found" >&2
	exit 1
}

VMLINUX_BIN="${IMAGES_DIR}/vmlinux.bin"
DTB="${IMAGES_DIR}/t23-openipc.dtb"
if [ ! -f "$DTB" ] && [ -f "${KDIR}/arch/mips/boot/dts/ingenic/t23-openipc.dtb" ]; then
	DTB="${KDIR}/arch/mips/boot/dts/ingenic/t23-openipc.dtb"
fi
VMLINUX_ELF="${KDIR}/vmlinux"
KREL_FILE="${KDIR}/include/config/kernel.release"

for f in "$VMLINUX_BIN" "$DTB" "$VMLINUX_ELF" "$KREL_FILE"; do
	[ -f "$f" ] || {
		echo "v4l2-post-image: ERROR: required file not found: $f" >&2
		exit 1
	}
done

vmlinux_sz=$(stat -c %s "$VMLINUX_BIN")
if [ "$vmlinux_sz" -ge 4718592 ]; then
	echo "v4l2-post-image: ERROR: vmlinux.bin ($vmlinux_sz bytes) >= 4718592 (4.5 MiB U-Boot decompression limit)" >&2
	exit 1
fi

LOADADDR=$("$READELF" -lW "$VMLINUX_ELF" | awk '$1 == "LOAD" { print $3; exit }')
ENTRY=$("$READELF" -h "$VMLINUX_ELF" | awk '/Entry point address:/ { print $NF }')
KREL=$(tr -d '-' < "$KREL_FILE")

if [ -z "$LOADADDR" ] || [ -z "$ENTRY" ] || [ -z "$KREL" ]; then
	echo "v4l2-post-image: ERROR: failed to read ELF load/entry or kernel.release" >&2
	exit 1
fi

TMP_BIN="${IMAGES_DIR}/vmlinux-dtb.bin.$$"
TMP_LZMA="${IMAGES_DIR}/vmlinux-dtb.lzma.$$"
cleanup() {
	rm -f "$TMP_BIN" "$TMP_LZMA"
}
trap cleanup EXIT HUP INT TERM

cat "$VMLINUX_BIN" "$DTB" > "$TMP_BIN"
"$XZ" --format=lzma -9 -c "$TMP_BIN" > "$TMP_LZMA"
"$MKIMAGE" -A mips -O linux -T kernel -C lzma \
	-a "$LOADADDR" -e "$ENTRY" \
	-n "Linux-${KREL}-t23" \
	-d "$TMP_LZMA" "${IMAGES_DIR}/uImage"

echo "v4l2-post-image: uImage $(( $(stat -c %s "${IMAGES_DIR}/uImage") / 1024 ))KB"
