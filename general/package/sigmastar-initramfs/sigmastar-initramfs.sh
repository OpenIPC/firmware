#!/bin/bash
BUSYBOX_SOURCE=busybox-1.36.0
BUSYBOX_SITE=https://www.busybox.net/downloads

DOSFSTOOLS_SOURCE=dosfstools-4.2
DOSFSTOOLS_SITE=https://github.com/dosfstools/dosfstools/releases/download/v4.2

TOOLCHAIN_SOURCE=cortex_a7_thumb2_hf-gcc8.4.0-musl-4_9-1e913a17
TOOLCHAIN_SITE=https://github.com/openipc/firmware/releases/download/latest

FILES=$1/initramfs/source
CROSS=$FILES/arm-openipc-linux-musleabihf_sdk-buildroot/bin

wget -c $BUSYBOX_SITE/$BUSYBOX_SOURCE.tar.bz2 -P $FILES
wget -c $DOSFSTOOLS_SITE/$DOSFSTOOLS_SOURCE.tar.gz -P $FILES
wget -c $TOOLCHAIN_SITE/$TOOLCHAIN_SOURCE.tgz -P $FILES

tar -xf $FILES/$BUSYBOX_SOURCE.tar.bz2 -C $FILES
tar -xf $FILES/$DOSFSTOOLS_SOURCE.tar.gz -C $FILES
tar -xf $FILES/$TOOLCHAIN_SOURCE.tgz -C $FILES

mv -f $FILES/../initramfs_defconfig $FILES/$BUSYBOX_SOURCE/.config
$2 CROSS_COMPILE=$CROSS/arm-linux- -C $FILES/$BUSYBOX_SOURCE
mv -f $FILES/$BUSYBOX_SOURCE/busybox $FILES/..

cd $FILES/$DOSFSTOOLS_SOURCE && ./autogen.sh && ./configure
$2 CC=$CROSS/arm-linux-gcc CFLAGS="-static -s" -C $FILES/$DOSFSTOOLS_SOURCE
mv -f $FILES/$DOSFSTOOLS_SOURCE/src/fsck.fat $FILES/..
