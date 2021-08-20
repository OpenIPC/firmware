#!/bin/bash
#
# OpenIPC.org | v.20210814
#

clone() {
  sudo apt-get update -y ; apt-get install -y bc build-essential git unzip
  git clone --depth=1 https://github.com/OpenIPC/openipc-2.1.git
}

fresh() {
  date >/tmp/openipc_buildtime.txt
  make distclean #clean
  [ -d buildroot* ] && echo -e "\nBuildroot found, OK\n" || make prepare
}

rename() {
  [[ $(stat --printf="%s" ./output/images/uImage) -gt 2097152 ]] && TG_NOTIFY="Warning: kernel size exceeded : $(stat --printf="%s" ./output/images/uImage) vs 2097152" && exit 1
  [[ $(stat --printf="%s" ./output/images/rootfs.squashfs) -gt 5242880 ]] && TG_NOTIFY="Warning: rootfs size exceeded - $(stat --printf="%s" ./output/images/rootfs.squashfs) vs 5242880" && exit 1
  #
  mv -v ./output/images/uImage ./output/images/uImage.${soc}
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.${soc}
  mv -v ./output/images/rootfs.tar ./output/images/rootfs.${soc}.tar
  date >>/tmp/openipc_buildtime.txt
  echo -e "\n\n$(cat /tmp/openipc_buildtime.txt)"
}

upload() {
  server="zig@172.28.200.74:/sync/Archive/Incoming/Tftp/"
  echo -e "\n\nStart transferring files to the TFTP server...\n"
  scp -P 22 -r ./output/images/rootfs.squashfs.* ./output/images/uImage.* ${server}
  echo -e "\n"
  date
}

sdk() {
  make br-sdk
}

fh8852() {
  soc="fh8852"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_fh8852_openipc all && rename
}

fh8856() {
  soc="fh8856"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_fh8856_openipc all && rename
}

hi3516cv100() {
  soc="hi3516cv100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_unknown all && rename
}

hi3516cv200() {
  soc="hi3516cv200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_unknown all && rename
}

hi3516cv300() {
  soc="hi3516cv300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_unknown all && rename
}

hi3516ev200() {
  soc="hi3516ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev200_openipc all && rename
}

hi3516ev300() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev300_openipc all && rename
}

hi3516ev300_glibc() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev300_glibc all && rename
}

hi3516ev300_tehshield() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_hi3516ev300_tehshield all && rename
}

#################################################################################

ssc325() {
  soc="ssc325"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc325_openipc all && rename
}

#################################################################################

ssc333() {
  soc="ssc333"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc333_openipc all && rename
}

ssc335() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335_openipc all && rename
}

ssc335_blackbird() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335_blackbird all && rename
}

ssc335_goodcam() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335_goodcam all && rename
}

ssc335_initramfs() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335_initramfs all && rename
}

ssc335_musl() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335_musl all && rename
}

ssc335_rotek() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335_rotek all && rename
}

ssc337() {
  soc="ssc337"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc337_openipc all && rename
}

#################################################################################

ssc335de() {
  soc="ssc335de"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc335de_openipc all && rename
}

ssc337de() {
  soc="ssc337de"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_ssc337de_openipc all && rename
}

#################################################################################

xm530() {
  soc="xm530"
  fresh && make PLATFORM=xiongmai BOARD=unknown_unknown_xm530_openipc all && rename
}



# Build firmware
#
# fh8852                        # testing..
# fh8856                        # testing..
#
# hi3516cv100                   # testing..
# hi3516cv200                   # testing..
# hi3516cv300                   # testind..
#
# hi3516ev200                   # OK
#
# hi3516ev300                   # OK
# hi3516ev300_glibc             # testing..
# hi3516ev300_tehshield         # Tehshield
#
# ssc325                        # OpenIPC
#
# ssc333                        # OpenIPC
# ssc335                        # OpenIPC
# ssc335_blackbird              # BlackBird
# ssc335_goodcam                # GoodCam
ssc335_initramfs              # Initramfs
# ssc335_musl                   # Musl
# ssc335_rotek                  # Rotek
# ssc337                        # OpenIPC
#
# ssc335de                      # OpenIPC
# ssc337de                      # OpenIPC
#
# xm530                         # OK
#
#
#
# More examples see here: https://github.com/OpenIPC/openipc-2.1/wiki/source_code
#
