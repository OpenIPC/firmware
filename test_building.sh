#!/bin/bash

#
clone() {
  sudo apt-get update -y ; apt-get install -y bc build-essential git unzip
  git clone --depth=1 https://github.com/OpenIPC/openipc-2.1.git
}

fresh() {
  make distclean
  #make clean
  [ -d buildroot* ] && echo -e "\nBuildroot found, OK\n" || make prepare
}

2dolphin() {
  server="zig@172.28.200.74:/sync/Archive/Incoming/Tftp/"
  echo -e "\n\nStart transferring files to the TFTP server...\n"
  scp -P 22 -r ./output/images/rootfs.squashfs.* ./output/images/uImage.* ${server}
  echo -e "\n"
  date
}

sdk() {
  make br-sdk
}

hi3516cv200() {
  make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516cv200_unknown_defconfig
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.hi3516cv200
  mv -v ./output/images/uImage ./output/images/uImage.hi3516cv200
}

hi3516ev200() {
  make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev200_openipc all
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.hi3516ev200
  mv -v ./output/images/uImage ./output/images/uImage.hi3516ev200
}

hi3516ev300() {
  make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev300_openipc all
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.hi3516ev300
  mv -v ./output/images/uImage ./output/images/uImage.hi3516ev300
}

hi3516ev300_dev() {
  make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev300_dev all
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.hi3516ev300
  mv -v ./output/images/uImage ./output/images/uImage.hi3516ev300
}

hi3516ev300_glibc() {
  make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev300_glibc all
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.hi3516ev300
  mv -v ./output/images/uImage ./output/images/uImage.hi3516ev300
}

hi3516ev300_tehshield() {
  make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev300_tehshield all
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.hi3516ev300
  mv -v ./output/images/uImage ./output/images/uImage.hi3516ev300
}

ssc335() {
  make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335_openipc all
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.ssc335
  mv -v ./output/images/uImage ./output/images/uImage.ssc335
}

ssc335_blackbird() {
  make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335_blackbird all
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.ssc335
  mv -v ./output/images/uImage ./output/images/uImage.ssc335
}

xm530() {
  make PLATFORM=xiongmai BOARD=unknown_unknown_xm530_openipc all
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.xm530
  mv -v ./output/images/uImage ./output/images/uImage.xm530
}



#
fresh; hi3516cv200
#
# fresh; hi3516ev200
#
# fresh; hi3516ev300
# fresh; hi3516ev300_dev
# fresh; hi3516ev300_glibc
# fresh; hi3516ev300_tehshield
#
# fresh; ssc335
# fresh; ssc335_blackbird
#
# fresh; xm530; 2dolphin
#
# 2dolphin
#

# Some examples
#
# make PLATFORM=sigmastar br-linux-{dirclean,rebuild}
#

