#!/bin/bash
#
# OpenIPC.org | v.20220515
#

MAX_KERNEL_SIZE=0x200000               #    2MiB,  2097152
MAX_KERNEL_SIZE_ULTIMATE=0x300000      #    3MiB,  3145728
MAX_KERNEL_SIZE_EXPERIMENTAL=0x3E8480  # ~3.9MiB,  4097152
MAX_ROOTFS_SIZE=0x500000               #    5MiB,  5242880
MAX_ROOTFS_SIZE_ULTIMATE=0xA00000      #   10MiB,  10485760

clone() {
  sudo apt-get update -y ; apt-get install -y bc build-essential git unzip rsync autotools-dev automake libtool
  git clone --depth=1 https://github.com/OpenIPC/firmware.git
}

fresh() {
  echo -e "\nThe start-stop times\n" >/tmp/openipc_buildtime.txt
  date >>/tmp/openipc_buildtime.txt
  [ -d buildroot-2020.02.12/dl ] && mv buildroot-2020.02.12/dl . && rm -rf ./dl/majestic* ./dl/ipctool
  make distclean #clean
  [ -d buildroot* ] && echo -e "\nBuildroot found, OK\n" || make prepare
  [ -d dl ] && mv dl buildroot-2020.02.12/dl || return 0
}

should_fit() {
  filename=$1
  maxsize=$2
  filesize=$(stat --printf="%s" ./output/images/$filename)
  if [[ $filesize -gt $maxsize ]]; then
    export TG_NOTIFY="Warning: $filename is too large: $filesize vs $maxsize"
    exit 1
  fi
}

rename() {
  if grep -q ultimate_defconfig ./output/.config || grep -q fpv_defconfig ./output/.config; then
      should_fit uImage $MAX_KERNEL_SIZE_ULTIMATE
      should_fit rootfs.squashfs $MAX_ROOTFS_SIZE_ULTIMATE
  else
      should_fit uImage $MAX_KERNEL_SIZE
      should_fit rootfs.squashfs $MAX_ROOTFS_SIZE
  fi
  mv -v ./output/images/uImage ./output/images/uImage.${soc}
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.${soc}
  mv -v ./output/images/rootfs.cpio ./output/images/rootfs.${soc}.cpio
  mv -v ./output/images/rootfs.tar ./output/images/rootfs.${soc}.tar
  date >>/tmp/openipc_buildtime.txt
}

rename_initramfs() {
  should_fit uImage $MAX_KERNEL_SIZE_EXPERIMENTAL
  mv -v ./output/images/uImage ./output/images/uImage.initramfs.${soc}
  mv -v ./output/images/rootfs.cpio ./output/images/rootfs.${soc}.cpio
  mv -v ./output/images/rootfs.tar ./output/images/rootfs.${soc}.tar
  date >>/tmp/openipc_buildtime.txt
  echo -e "\n\n$(cat /tmp/openipc_buildtime.txt)"
}

autoup_rootfs() {
  echo -e "\n"
  curl -L -o ./output/images/u-boot-hi3518ev200-universal.bin https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3518ev200-universal.bin
  echo -e "\n"
  ./output/host/bin/mkimage -A arm -O linux -T firmware -n 'OpenIPC v.2.2.7' -a 0x000000000000 -e 0x000000050000 -d ./output/images/u-boot-hi3518ev200-universal.bin ./output/images/autoupdate-uboot.img
  echo -e "\n"
  ./output/host/bin/mkimage -A arm -O linux -T kernel -C none -n 'OpenIPC v2.2.7' -a 0x000000050000 -e 0x000000250000 -d ./output/images/uImage.${soc} ./output/images/autoupdate-kernel.img
  echo -e "\n"
  ./output/host/bin/mkimage -A arm -O linux -T filesystem -n 'OpenIPC v.2.2.7' -a 0x000000250000 -e 0x000000750000 -d ./output/images/rootfs.squashfs.${soc} ./output/images/autoupdate-rootfs.img
}

sdk() {
  make br-sdk
}

upload() {
  TFTP_SERVER="root@172.17.32.17:/mnt/bigger-2tb/Rotator/TFTP"
  echo -e "\n\nStart transferring files to the TFTP server...\n"
  scp -P 22 -r ./output/images/rootfs.squashfs.* ./output/images/uImage.* ${TFTP_SERVER}
}

tg_message() {
  #TG_TOKEN=
  #TG_CHANNEL=
  TG_REPLY="${TG_REPLY:=No comment.\nAll details will come later.}"
  TG_MESSAGE=$(echo -e "\xF0\x9F\x8C\x8D PARADOX" "%0A${TG_REPLY}")
  #
  curl -s -k --connect-timeout 30 --max-time 30 -X POST \
    https://api.telegram.org/bot${TG_TOKEN}/sendMessage -d chat_id="${TG_CHANNEL}" -d disable_notification="true" -d text="${TG_MESSAGE}" >/dev/null 2>&1
}

#################################################################################

ambarella-s3l() {
  soc="s3l"
  fresh && make PLATFORM=ambarella BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

ak3918ev200() {
  soc="ak3918ev200"
  fresh && make PLATFORM=anyka BOARD=unknown_unknown_${soc}_openipc all && rename
}

ak3916ev300() {
  soc="ak3916ev300"
  fresh && make PLATFORM=anyka BOARD=unknown_unknown_${soc}_openipc all && rename
}

ak3918ev300() {
  soc="ak3918ev300"
  fresh && make PLATFORM=anyka BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

fh8833v100() {
  soc="fh8833v100"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

fh8852v100() {
  soc="fh8852v100"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

fh8852v200() {
  soc="fh8852v200"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

fh8852v210() {
  soc="fh8852v210"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

fh8856v100() {
  soc="fh8856v100"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

fh8856v200() {
  soc="fh8856v200"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

fh8856v210() {
  soc="fh8856v210"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

fh8858v200() {
  soc="fh8858v200"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

fh8858v210() {
  soc="fh8858v210"
  fresh && make PLATFORM=fullhan BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

gk7101() {
  soc="gk7101"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_openipc all && rename
}

gk7101s() {
  soc="gk7101s"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_openipc all && rename
}

gk7102() {
  soc="gk7102"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_openipc all && rename
}

gk7102s() {
  soc="gk7102s"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_openipc all && rename
}

gk7205v200() {
  soc="gk7205v200"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_openipc all && rename
}

gk7205v200_ultimate() {
  soc="gk7205v200"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_ultimate all && rename
}

gk7205v200_fpv() {
  soc="gk7205v200"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_fpv all && rename
}

gk7205v200_iscom() {
  soc="gk7205v200"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_iscom all && rename
}

gk7205v200_ufanet() {
  soc="gk7205v200"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_ufanet all && rename
}

gk7205v210() {
  soc="gk7205v210"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_openipc all && rename
}

gk7205v300() {
  soc="gk7205v300"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_openipc all && rename
}

gk7205v300_ultimate() {
  soc="gk7205v300"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_ultimate all && rename
}

gk7205v300_fpv() {
  soc="gk7205v300"
  fresh && make PLATFORM=goke BOARD=unknown_unknown_${soc}_fpv all && rename
}

#################################################################################

gm8135() {
  soc="gm8135"
  fresh && make PLATFORM=grainmedia BOARD=unknown_unknown_${soc}_openipc all && rename
}

gm8136() {
  soc="gm8136"
  fresh && make PLATFORM=grainmedia BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

hi3516cv100() {
  soc="hi3516cv100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3518ev100() {
  soc="hi3518ev100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

hi3516cv200() {
  soc="hi3516cv200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3518ev200() {
  soc="hi3518ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3518ev200_domsip() {
  soc="hi3518ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_domsip all && rename
}

hi3518ev200_hs303() {
  soc="hi3518ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename && autoup_rootfs
  #PLATFORM=hisilicon  make br-linux-{dirclean,rebuild}
  #PLATFORM=hisilicon  make br-mbedtls-openipc-{dirclean,rebuild}
}

hi3518ev200_ultimate() {
  soc="hi3518ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_ultimate all && rename
}

#################################################################################

hi3516cv300() {
  soc="hi3516cv300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516cv300_ultimate() {
  soc="hi3516cv300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_ultimate all && rename
}

hi3516ev100() {
  soc="hi3516ev100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}
#################################################################################

hi3516av100() {
  soc="hi3516av100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516av100_ultimate() {
  soc="hi3516av100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_ultimate all && rename
}

hi3516dv100() {
  soc="hi3516dv100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

hi3519v101() {
  soc="hi3519v101"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516av200() {
  soc="hi3516av200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516av200_ultimate() {
  soc="hi3516av200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_ultimate all && rename
}

#################################################################################

hi3516av300() {
  soc="hi3516av300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516cv500() {
  soc="hi3516cv500"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516dv300() {
  soc="hi3516dv300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

hi3516dv200() {
  soc="hi3516dv200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516ev200() {
  soc="hi3516ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516ev200_dozor() {
  soc="hi3516ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_dozor all && rename
}

hi3516ev200_eltis() {
  soc="hi3516ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_eltis all && rename
}

hi3516ev200_vixand() {
  soc="hi3516ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_vixand all && rename
}

hi3516ev200_ultimate() {
  soc="hi3516ev200"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_ultimate all && rename
}

hi3516ev300() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3516ev300_dev() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_dev all && rename
}

hi3516ev300_fpv() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_fpv all && rename
}

hi3516ev300_glibc() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_glibc all && rename
}

hi3516ev300_tehshield() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_tehshield all && rename
}

hi3516ev300_ultimate() {
  soc="hi3516ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_ultimate all && rename
}

hi3518ev300() {
  soc="hi3518ev300"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3536cv100() {
  soc="hi3536cv100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3536dv100() {
  soc="hi3536dv100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_openipc all && rename
}

hi3536dv100_vixand() {
  soc="hi3536dv100"
  fresh && make PLATFORM=hisilicon BOARD=unknown_unknown_${soc}_vixand all && rename
}

#################################################################################

msc313e() {
  soc="msc313e"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

msc313e_baresip() {
  soc="msc313e"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_baresip all && rename
}

msc316dc() {
  soc="msc316dc"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

msc316dm() {
  soc="msc316dm"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

nt98562() {
  soc="nt98562"
  fresh && make PLATFORM=novatek BOARD=unknown_unknown_${soc}_openipc all && rename
  #PLATFORM=novatek make br-linux-{dirclean,rebuild}
}

nt98566() {
  soc="nt98566"
  fresh && make PLATFORM=novatek BOARD=unknown_unknown_${soc}_openipc all && rename
}

nt98566_polcam() {
  soc="nt98566"
  fresh && make PLATFORM=novatek BOARD=unknown_unknown_${soc}_polcam all && rename
}

#################################################################################

rv1109() {
  soc="rv1109"
  fresh && make PLATFORM=rockchip BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

rv1126() {
  soc="rv1126"
  fresh && make PLATFORM=rockchip BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

ssc325() {
  soc="ssc325"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

ssc333() {
  soc="ssc333"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

ssc335() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

ssc335_blackbird() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_blackbird all && rename
}

ssc335_goodcam() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_goodcam all && rename
}

ssc335_initramfs() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_initramfs all && rename_initramfs
}

ssc335_musl() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_musl all && rename
}

ssc335_portal() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_portal all && rename
}

ssc335_rotek() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_rotek all && rename
}

ssc335_tiandy() {
  soc="ssc335"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_tiandy all && rename
}

ssc337() {
  soc="ssc337"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

ssc337_kama() {
  soc="ssc337"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_kama all && rename
}

#################################################################################

ssc335de() {
  soc="ssc335de"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

ssc337de() {
  soc="ssc337de"
  fresh && make PLATFORM=sigmastar BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

t10() {
  soc="t10"
  fresh && make PLATFORM=ingenic BOARD=unknown_unknown_${soc}_openipc all && rename
}

t20() {
  soc="t20"
  fresh && make PLATFORM=ingenic BOARD=unknown_unknown_${soc}_openipc all && rename
}

t30() {
  soc="t30"
  fresh && make PLATFORM=ingenic BOARD=unknown_unknown_${soc}_openipc all && rename
}

t31() {
  soc="t31"
  fresh && make PLATFORM=ingenic BOARD=unknown_unknown_${soc}_openipc all && rename
}

t31_vixand() {
  soc="t31"
  fresh && make PLATFORM=ingenic BOARD=unknown_unknown_${soc}_vixand all && rename
}

#################################################################################

xm510() {
  soc="xm510"
  fresh && make PLATFORM=xiongmai BOARD=unknown_unknown_${soc}_openipc all && rename
}

xm530() {
  soc="xm530"
  fresh && make PLATFORM=xiongmai BOARD=unknown_unknown_${soc}_openipc all && rename
}

xm550() {
  soc="xm550"
  fresh && make PLATFORM=xiongmai BOARD=unknown_unknown_${soc}_openipc all && rename
}

#################################################################################

#################################################################################

# Build firmware
#######
#
# ambarella-s3l                   # testing..
#
#######
#
# ak3918ev200                   # testing..
# ak3916ev300                   # testing..
# ak3918ev300                   # testing..
#
#######
#
# fh8833v100                    # testing..
# fh8852v100                    # testing..
# fh8852v200                    # testing..
# fh8852v210                    # testing..
# fh8856v100                    # testing..
# fh8856v200                    # testing..
# fh8856v210                    # testing..
# fh8858v200                    # testing..
# fh8858v210                    # testing..
#
#######
#
# gm8135                        # testing..
# gm8136                        # testing..
#
#######
#
# gk7101                        # testing..
# gk7101s                       # testing..
# gk7102                        # testing..
# gk7102s                       # testing..
#
#######
#
# gk7202v300                    # testing..
# gk7205v200                    # OpenIPC
# gk7205v200_ultimate           # OpenIPC_ultimate version
# gk7205v200_fpv                # FPV (ultimate by default)
# gk7205v200_iscom              # Iscom test
# gk7205v200_ufanet             # Ufanet
# gk7205v210                    # OpenIPC
# gk7205v300                    # OpenIPC
# gk7205v300_ultimate           # OpenIPC_ultimate version
gk7205v300_fpv                # FPV (ultimate by default)
# gk7605v100                    # testing..
#
#######
#
# hi3516cv100                   # OpenIPC
# hi3518ev100                   # OpenIPC
#
# hi3516cv200                   # testing..
# hi3518ev200                   # testing..
# hi3516cv200                   # testing..
# hi3518ev200                   # testing..
# hi3518ev200_domsip            # DomSip
# hi3518ev200_hs303             # OpenIPC
# hi3518ev200_ultimate          # OpenIPC
#
# hi3516av100                   # OpenIPC
# hi3516av100_ultimate          # OpenIPC_ultimate version
# hi3516dv100                   # OpenIPC
#
# hi3516cv300                   # OpenIPC
# hi3516cv300_ultimate          # OpenIPC_ultimate version
# hi3516ev100                   # OpenIPC
#
# hi3516dv200                   # OpenIPC
# hi3516ev200                   # OpenIPC
# hi3516ev200_dozor             # Dozor
# hi3516ev200_eltis             # Eltis
# hi3516ev200_vixand            # Vixand
# hi3516ev200_ultimate          # OpenIPC_ultimate version
# hi3516ev300                   # OpenIPC
# hi3516ev300_dev               # OpenIPC development
# hi3516ev300_fpv               # FPV (ultimate by default)
# hi3516ev300_glibc             # testing..
# hi3516ev300_tehshield         # Tehshield
# hi3516ev300_ultimate          # OpenIPC_ultimate version
# hi3518ev300                   # OpenIPC
#
# hi3519v101                    # OpenIPC
# hi3516av200                   # OpenIPC
# hi3516av200_ultimate          # OpenIPC_ultimate version
#
# hi3516av300                   # testing..
# hi3516cv500                   # testing..
# hi3516dv300                   # testing..
#
# hi3536cv100                   # OpenIPC
# hi3536dv100                   # OpenIPC
# hi3536dv100_vixand            # Vixand
#
#######
#
# msc313e                       # OpenIPC
# msc313e_baresip               # OpenIPC
# msc316dc                      # OpenIPC
# msc316dm                      # OpenIPC
#
#######
#
# nt98562                       # OpenIPC
# nt98566                       # OpenIPC
# nt98566_polcam                # Polcam
#
#######
#
# rv1109                        # testing..
# rv1126                        # testing..
#
#######
#
# ssc325                        # OpenIPC
#
# ssc333                        # OpenIPC
#
# ssc335                        # OpenIPC
# ssc335_blackbird              # BlackBird
# ssc335_goodcam                # GoodCam
# ssc335_initramfs              # Initramfs
# ssc335_musl                   # Musl
# ssc335_portal                 # Portal (partner)
# ssc335_rotek                  # Rotek
# ssc335_tiandy                 # Tiandy
#
# ssc335de                      # OpenIPC
#
# ssc337                        # OpenIPC => musl
# ssc337_kama                   # uClibc
#
# ssc337de                      # OpenIPC
#
#######
#
# t10                           # OpenIPC
# t20                           # OpenIPC
# t30                           # OpenIPC
# t31                           # OpenIPC
# t31_vixand                    # Vixand partner
#
#######
#
# xm510                         # testing
# xm530                         # OK
# xm550                         # OK
#
#######
#
# More examples see here: https://openipc.github.io/wiki/
#

echo -e "\n\n$(cat /tmp/openipc_buildtime.txt)"

