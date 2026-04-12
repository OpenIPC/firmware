#!/bin/sh
link=https://github.com/openipc/firmware/releases/download/latest

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
	echo "Usage: $0 [uboot] [firmware] [variant]"
	echo "  $0 t21n t21 ultimate"
	echo "  $0 ssc335 ssc335 lite"
	exit 0
fi

uboot=u-boot-$1-nor.bin
firmware=openipc.$2-nor-$3.tgz
release=openipc-$2-nor.bin

mkdir -p output
if ! wget -q --show-progress $link/$uboot -O output/$1.bin; then
	echo "Download failed: $link/$uboot"
	exit 1
fi

if ! wget -q --show-progress $link/$firmware -O output/$2.tgz; then
	echo "Download failed: $link/$firmware"
	exit 1
fi

tar -xf output/$2.tgz -C output
dd if=/dev/zero bs=1K count=5000 status=none | tr '\000' '\377' > $release
dd if=output/$1.bin of=$release bs=1K seek=0 conv=notrunc status=none
dd if=output/uImage.$2 of=$release bs=1K seek=320 conv=notrunc status=none
dd if=output/rootfs.squashfs.$2 of=$release bs=1K seek=2368 conv=notrunc status=none
rm -rf output

echo "Created: $release"
