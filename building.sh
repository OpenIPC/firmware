#!/bin/bash
#
# OpenIPC.org (c)
#

#
# Constants
#

MAX_KERNEL_SIZE=0x200000              #    2MiB,  2097152
MAX_KERNEL_SIZE_ULTIMATE=0x300000     #    3MiB,  3145728
MAX_KERNEL_SIZE_EXPERIMENTAL=0x3E8480 # ~3.9MiB,  4097152
MAX_ROOTFS_SIZE=0x500000              #    5MiB,  5242880
MAX_ROOTFS_SIZE_ULTIMATE=0xA00000     #   10MiB, 10485760

_d=$(date +"%y.%m.%d")
OPENIPC_VER=$(echo OpenIPC v${_d:0:1}.${_d:1})
unset _d

SRC_CACHE_DIR="/tmp/buildroot_dl"

#
# Functions
#

echo_c() {
  # 30 grey, 31 red, 32 green, 33 yellow, 34 blue, 35 magenta, 36 cyan,37 white
  echo -e "\e[1;$1m$2\e[0m"
}

log_and_run() {
  _command=$1
  echo_c 35 "$_command"
  $_command
  unset _command
}

clone() {
  sudo apt-get update -y
  sudo apt-get install -y automake autotools-dev bc build-essential curl fzf git libtool rsync unzip
  git clone --depth=1 https://github.com/OpenIPC/firmware.git
}

fresh() {
  BR_VER=$1

  if [ -d "$SRC_CACHE_DIR" ]; then
    echo_c 36 "Found cache directory."
  else
    echo_c 31 "Cache directory not found."
    echo_c 34 "Creating cache directory ..."
    log_and_run "mkdir -p ${SRC_CACHE_DIR}"
    echo_c 34 "Done.\n"
  fi

  if [ -d "buildroot-${BR_VER}" ]; then
    echo_c 36 "Found existing Buildroot directory."

    if [ -d "buildroot-${BR_VER}/dl" ]; then
      echo_c 36 "Found existing Buildroot downloads directory."
      echo_c 34 "Copying Buildroot downloads to cache directory ..."
      log_and_run "cp -rvf buildroot-${BR_VER}/dl/* ${SRC_CACHE_DIR}"
      echo_c 34 "Done.\n"
    fi

    echo_c 34 "Cleaning source directory."
    echo_c 35 "make distclean"
    make distclean
    echo_c 34 "Done.\n"
  else
    echo_c 31 "Buildroot sources not found."
  fi

  echo_c 34 "Downloading Buildroot sources to cache directory ..."
  log_and_run "curl --continue-at - --output ${SRC_CACHE_DIR}/buildroot-${BR_VER}.tar.gz https://buildroot.org/downloads/buildroot-${BR_VER}.tar.gz"
  echo_c 34 "Done.\n"

  echo_c 34 "Extracting a fresh copy of Buildroot from Buildroot sources ..."
  log_and_run "tar xvf ${SRC_CACHE_DIR}/buildroot-${BR_VER}.tar.gz"
  echo_c 34 "Done.\n"

  echo_c 34 "Copying cached source files back to Buildroot ..."
  log_and_run "mkdir -p buildroot-${BR_VER}/dl/"
  log_and_run "cp -rvf ${SRC_CACHE_DIR}/* buildroot-${BR_VER}/dl/"
  echo_c 34 "Done.\n"

  # make prepare

  echo_c 33 "Start building OpenIPC Firmware ${OPENIPC_VER} for ${SOC}."
  echo "The start-stop times" >/tmp/openipc_buildtime.txt
  date >>/tmp/openipc_buildtime.txt
}

should_fit() {
  filename=$1
  maxsize=$2
  filesize=$(stat --printf="%s" ./output/images/$filename)
  if [[ $filesize -gt $maxsize ]]; then
    export TG_NOTIFY="Warning: $filename is too large: $filesize vs $maxsize"
    echo_c 31 "Warning: $filename is too large: $filesize vs $maxsize"
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
  echo_c 31 "\n\n$(cat /tmp/openipc_buildtime.txt)\n\n"
}

rename_initramfs() {
  should_fit uImage $MAX_KERNEL_SIZE_EXPERIMENTAL
  mv -v ./output/images/uImage ./output/images/uImage.initramfs.${SOC}
  mv -v ./output/images/rootfs.cpio ./output/images/rootfs.${SOC}.cpio
  mv -v ./output/images/rootfs.tar ./output/images/rootfs.${SOC}.tar
  date >>/tmp/openipc_buildtime.txt
  echo_c 31 "\n\n$(cat /tmp/openipc_buildtime.txt)\n\n"
}

autoup_rootfs() {
  echo_c 34 "\nDownloading u-boot created by OpenIPC"
  curl --location --output ./output/images/u-boot-${SOC}-universal.bin \
    https://github.com/OpenIPC/firmware/releases/download/latest/u-boot-${SOC}-universal.bin

  echo_c 34 "\nMaking autoupdate u-boot image"
  ./output/host/bin/mkimage -A arm -O linux -T firmware -n "$OPENIPC_VER" \
    -a 0x0 -e 0x50000 -d ./output/images/u-boot-${SOC}-universal.bin \
    ./output/images/autoupdate-uboot.img

  echo_c 34 "\nMaking autoupdate kernel image"
  ./output/host/bin/mkimage -A arm -O linux -T kernel -C none -n "$OPENIPC_VER" \
    -a 0x50000 -e 0x250000 -d ./output/images/uImage.${SOC} \
    ./output/images/autoupdate-kernel.img

  echo_c 34 "\nMaking autoupdate rootfs image"
  ./output/host/bin/mkimage -A arm -O linux -T filesystem -n "$OPENIPC_VER" \
    -a 0x250000 -e 0x750000 -d ./output/images/rootfs.squashfs.${SOC} \
    ./output/images/autoupdate-rootfs.img
}

#################################################################################

FUNCS=(
  ambarella-s3l

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

  gk7202v300
  gk7205v200  gk7205v200_fpv  gk7205v200_ultimate
  gk7205v210
  gk7205v300  gk7205v300_fpv  gk7205v300_ultimate
  gk7605v100

  gm8135
  gm8136

  hi3516cv100
  hi3518ev100

  hi3516cv200
  hi3518ev200  hi3518ev200_ultimate

  hi3516cv300  hi3516cv300_ultimate
  hi3516ev100

  hi3516av100  hi3516av100_ultimate
  hi3516dv100  hi3516dv100_ultimate

  hi3519v101
  hi3516av200  hi3516av200_ultimate

  hi3516av300
  hi3516cv500
  hi3516dv300

  hi3516dv200
  hi3516ev200  hi3516ev200_fpv  hi3516ev200_ultimate  hi3516ev200_eltis
  hi3516ev300  hi3516ev300_fpv  hi3516ev300_ultimate  hi3516ev300_dev  hi3516ev300_glibc  hi3516ev300_tehshield
  hi3518ev300  hi3518ev300_ultimate

  hi3536cv100
  hi3536dv100  hi3536dv100_vixand

  msc313e  msc313e_baresip
  msc316dc
  msc316dm

  nt98562
  nt98566

  rv1109
  rv1126

  ssc325
  ssc333
  ssc335  ssc335_blackbird  ssc335_goodcam  ssc335_initramfs  ssc335_musl  ssc335_portal  ssc335_rotek  ssc335_tiandy
  ssc337  ssc337_kama

  ssc335de
  ssc337de

  t10
  t20
  t30
  t31  t31_ultimate

  xm510
  xm530
  xm550
)

copy_function() {
  test -n "$(declare -f "$1")" || return
  eval "${_/$1/$2}"
}

uni_build() {
  [ -z "$BOARD" ] && BOARD=$FUNCNAME

  SOC=$(echo $BOARD | cut -d '_' -f 1)

  set -e
  if [ "$(echo $BOARD | cut -sd '_' -f 2)" == "" ]; then
    BOARD="${BOARD}_lite"
  fi

  if [ "$BOARD" == "hi3518ev200_lite" ]; then
    NEED_AUTOUP=1
  fi

  echo_c 33 "\n  SoC: $SOC\nBoard: $BOARD\n"

  if [ "all" = "${COMMAND}" ]; then
    fresh $(make BOARD=${BOARD} buildroot-version)
  fi

  log_and_run "make BOARD=${BOARD} ${COMMAND}"

  if [ "all" = "${COMMAND}" ]; then
    if [ "$BOARD" == "ssc335_initramfs" ]; then
      rename_initramfs
    else
      rename
    fi

    if [ ! -z "$NEED_AUTOUP" ]; then
      autoup_rootfs
    fi
  fi
}

for i in "${FUNCS[@]}"; do
  copy_function uni_build $i
done

#######

if [ $# -eq 0 ]; then
  if ! command -v fzf >/dev/null 2>&1; then
    echo -ne "Usage: $0 <variant>\nVariants:"
    for i in "${FUNCS[@]}"; do echo -n " ${i}"; done
    echo
    exit 1
  else
    SELECTED=$(find . -path "*/br-ext-chip-*" -name "*_defconfig" | fzf)
    [ -z "$SELECTED" ] && exit 1
    BOARD=$(echo $SELECTED | cut -d / -f 4 | awk -F_ '{printf "%s_%s", $1, $2}')
  fi
else
  BOARD=$1
fi

COMMAND=$2
[ -z "$COMMAND" ] && COMMAND=all

echo_c 37 "Building OpenIPC for ${BOARD}"
uni_build $BOARD $COMMAND
