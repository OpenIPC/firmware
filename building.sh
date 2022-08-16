#!/bin/bash
#
# OpenIPC.org (c)
#

BR_VER=2021.02.12

MAX_KERNEL_SIZE=0x200000              #    2MiB,  2097152
MAX_KERNEL_SIZE_ULTIMATE=0x300000     #    3MiB,  3145728
MAX_KERNEL_SIZE_EXPERIMENTAL=0x3E8480 # ~3.9MiB,  4097152
MAX_ROOTFS_SIZE=0x500000              #    5MiB,  5242880
MAX_ROOTFS_SIZE_ULTIMATE=0xA00000     #   10MiB,  10485760

fresh() {
  echo -e "\nThe start-stop times\n" >/tmp/openipc_buildtime.txt
  date >>/tmp/openipc_buildtime.txt
  [ -d buildroot-${BR_VER}/dl ] && mv buildroot-${BR_VER}/dl .
  rm -rf ./dl/majestic ./dl/ipctool ./dl/microbe-web
  make distclean #clean
  [ -d buildroot-${BR_VER} ] && echo -e "\nBuildroot found, OK\n" || make prepare
  [ -d dl ] && mv dl buildroot-${BR_VER}/dl || return 0
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
  mv -v ./output/images/uImage ./output/images/uImage.${SOC}
  mv -v ./output/images/rootfs.squashfs ./output/images/rootfs.squashfs.${SOC}
  mv -v ./output/images/rootfs.cpio ./output/images/rootfs.${SOC}.cpio
  mv -v ./output/images/rootfs.tar ./output/images/rootfs.${SOC}.tar
  date >>/tmp/openipc_buildtime.txt
}

rename_initramfs() {
  should_fit uImage $MAX_KERNEL_SIZE_EXPERIMENTAL
  mv -v ./output/images/uImage ./output/images/uImage.initramfs.${SOC}
  mv -v ./output/images/rootfs.cpio ./output/images/rootfs.${SOC}.cpio
  mv -v ./output/images/rootfs.tar ./output/images/rootfs.${SOC}.tar
  date >>/tmp/openipc_buildtime.txt
  echo -e "\n\n$(cat /tmp/openipc_buildtime.txt)"
}

autoup_rootfs() {
  echo -e "\n"
  curl -L -o ./output/images/u-boot-hi3518ev200-universal.bin https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-hi3518ev200-universal.bin
  echo -e "\n"
  D=$(date "+%y.%m.%d")
  RELEASE=$(echo OpenIPC v${D:0:1}.${D:1})
  ./output/host/bin/mkimage -A arm -O linux -T firmware -n $RELEASE -a 0x0 -e 0x50000 -d ./output/images/u-boot-hi3518ev200-universal.bin ./output/images/autoupdate-uboot.img
  echo -e "\n"
  ./output/host/bin/mkimage -A arm -O linux -T kernel -C none -n $RELEASE -a 0x50000 -e 0x250000 -d ./output/images/uImage.${SOC} ./output/images/autoupdate-kernel.img
  echo -e "\n"
  ./output/host/bin/mkimage -A arm -O linux -T filesystem -n $RELEASE -a 0x250000 -e 0x750000 -d ./output/images/rootfs.squashfs.${SOC} ./output/images/autoupdate-rootfs.img
}

#################################################################################

FUNCS=(
  ambarella-s3l

  ak3918ev200
  ak3916ev300
  ak3918ev300

  fh8833v100
  fh8852v100
  fh8852v200
  fh8852v210
  fh8856v100
  fh8856v200
  fh8856v210
  fh8858v200
  fh8858v210

  gk7101
  gk7101s
  gk7102
  gk7102s

  gk7205v200
  gk7205v200_ultimate
  gk7205v200_fpv
  gk7205v210
  gk7205v300
  gk7205v300_ultimate
  gk7205v300_fpv
  gk7605v100

  gm8135
  gm8136

  hi3516cv100
  hi3518ev100

  hi3516cv200
  hi3518ev200
  hi3518ev200_hs303
  hi3518ev200_ultimate

  hi3516cv300
  hi3516cv300_ultimate
  hi3516ev100

  hi3516av100
  hi3516av100_ultimate
  hi3516dv100
  hi3516dv100_ultimate

  hi3519v101
  hi3516av200
  hi3516av200_ultimate

  hi3516av300
  hi3516cv500
  hi3516dv300

  hi3516dv200
  hi3516ev200
  hi3516ev200_ultimate
  hi3516ev300
  hi3516ev300_dev
  hi3516ev300_fpv
  hi3516ev300_glibc
  hi3516ev300_tehshield
  hi3516ev300_ultimate
  hi3518ev300
  hi3518ev300_ultimate

  hi3536cv100
  hi3536dv100
  hi3536dv100_vixand

  msc313e
  msc313e_baresip
  msc316dc
  msc316dm

  nt98562
  nt98566
  nt98566_polcam

  rv1109
  rv1126

  ssc325
  ssc333
  ssc335
  ssc335_blackbird
  ssc335_goodcam
  ssc335_initramfs
  ssc335_musl
  ssc335_portal
  ssc335_rotek
  ssc335_tiandy
  ssc337
  ssc337_kama

  ssc335de
  ssc337de

  t10
  t20
  t30
  t31
  t31_vixand

  xm510
  xm530
  xm550
)

copy_function() {
  test -n "$(declare -f "$1")" || return
  eval "${_/$1/$2}"
}

uni_build() {
  BOARD=$FUNCNAME
  SOC=$(echo $BOARD | cut -sd '_' -f 1)

  set -e
  if [ "$(echo $BOARD | cut -sd '_' -f 2)" == "" ]; then
    BOARD="${BOARD}_openipc"
  elif [ "$BOARD" == "hi3518ev200_hs303" ]; then
    BOARD=hi3518ev200_openipc
    NEED_AUTOUP=1
  fi

  fresh
  make BOARD=unknown_unknown_${BOARD} all
  if [ "$BOARD" == "ssc335_initramfs" ]; then
    rename_initramfs
  else
    rename
  fi

  if [ ! -z "$NEED_AUTOUP" ]; then
    autoup_rootfs
  fi
}

for i in "${FUNCS[@]}"; do
  copy_function uni_build $i
done

#################################################################################

#################################################################################

# Build firmware
#######
#
# ambarella-s3l                 # testing..
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
# gk7205v210                    # OpenIPC
# gk7205v300                    # OpenIPC
# gk7205v300_ultimate           # OpenIPC_ultimate version
# gk7205v300_fpv                # FPV (ultimate by default)
# gk7605v100                    # testing..
#
#######
#
# hi3516cv100                   # OpenIPC
# hi3518ev100                   # OpenIPC
#
# hi3516cv200                   # testing..
# hi3516cv200                   # testing..
# hi3518ev200                   # OpenIPC
# hi3518ev200_hs303             # OpenIPC
# hi3518ev200_ultimate          # OpenIPC_ultimate version
#
# hi3516av100                   # OpenIPC
# hi3516av100_ultimate          # OpenIPC_ultimate version
# hi3516dv100                   # OpenIPC
# hi3516dv100_ultimate          # OpenIPC_ultimate version
#
# hi3516cv300                   # OpenIPC
# hi3516cv300_ultimate          # OpenIPC_ultimate version
# hi3516ev100                   # OpenIPC
#
# hi3516dv200                   # OpenIPC
# hi3516ev200                   # OpenIPC
# hi3516ev200_fpv               # FPV (ultimate by default)
# hi3516ev200_eltis             # Eltis
# hi3516ev200_ultimate          # OpenIPC_ultimate version
# hi3516ev300                   # OpenIPC
# hi3516ev300_dev               # OpenIPC development
# hi3516ev300_fpv               # FPV (ultimate by default)
# hi3516ev300_glibc             # testing..
# hi3516ev300_tehshield         # Tehshield
# hi3516ev300_ultimate          # OpenIPC_ultimate version
# hi3518ev300                   # OpenIPC
# hi3518ev300_ultimate          # OpenIPC_ultimate version
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
