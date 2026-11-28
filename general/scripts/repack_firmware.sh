#!/bin/sh
link=https://github.com/openipc/firmware/releases/download/latest

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
	echo "Usage: $0 [uboot] [firmware] [variant] [nor_mib]"
	echo "  $0 t21n t21 ultimate"
	echo "  $0 ssc335 ssc335 lite"
	echo "  nor_mib: flash size, 8 for lite and 16 for ultimate by default"
	exit 0
fi

uboot=u-boot-$1-nor.bin
release=openipc-$2-nor.bin
case "$4" in
	"") [ "$3" = ultimate ] && nor=16 || nor=8 ;;
	*) nor=$4 ;;
esac

mkdir -p output
if ! wget -q --show-progress $link/$uboot -O output/$1.bin; then
	echo "Download failed: $link/$uboot"
	exit 1
fi

# The split package (firmware.bin: the uImage, then the squashfs at the next
# 64 KiB) is what a board on the split layout ships. A board whose SoC family
# has no split kernel ships the plain package, with uImage and squashfs apart.
if wget -q --show-progress $link/openipc.$2-nor-$3-split.tgz -O output/$2.tgz; then
	tar -xf output/$2.tgz -C output
	dd if=/dev/zero bs=1M count=$nor status=none | tr '\000' '\377' > $release
	dd if=output/$1.bin of=$release bs=1K seek=0 conv=notrunc status=none
	dd if=output/firmware.bin.$2 of=$release bs=1K seek=320 conv=notrunc status=none
elif wget -q --show-progress $link/openipc.$2-nor-$3.tgz -O output/$2.tgz; then
	tar -xf output/$2.tgz -C output
	dd if=/dev/zero bs=1K count=5000 status=none | tr '\000' '\377' > $release
	dd if=output/$1.bin of=$release bs=1K seek=0 conv=notrunc status=none
	dd if=output/uImage.$2 of=$release bs=1K seek=320 conv=notrunc status=none
	dd if=output/rootfs.squashfs.$2 of=$release bs=1K seek=2368 conv=notrunc status=none
else
	echo "Download failed: no openipc.$2-nor-$3[-split].tgz at $link"
	exit 1
fi
rm -rf output

echo "Created: $release"
