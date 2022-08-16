#!/usr/bin/env bash

shopt -s nullglob

toolchain_by_config() {
  CF=$1
  FMT=$2

  BR2_VER=$(grep BR2_DEFAULT_KERNEL_VERSION $CF)
  if [ ! -z $BR2_VER ]; then
    VENDOR=$(echo $CF | cut -d - -f 4 | cut -d / -f 1 | sed -r 's/\<./\U&/g')
    VER=$(echo $BR2_VER | cut -d \" -f 2 | awk -F . '{printf "%s_%s", $1, $2}')
    GCC_VER=$(sed -rn "s/^BR2_GCC_VERSION_([0-9]*)_X=y/\1/p" $CF)
    ARCH=$(sed -rn "s/^BR2_(arm[a-z0-9_]+)=y/\1/p" $CF)
    if [ -z "$ARCH" ]; then
      ARCH=$(sed -rn "s/^BR2_(cortex_[a-z0-9_]+)=y/\1/p" $CF)
    fi
    if [ -z "$ARCH" ]; then
      ARCH=$(sed -rn "s/^BR2_(arm)=y/\1/p" $CF)
    else
      # Here we have something from ARM modern family
      HF=$(sed -rn "s/^BR2_ARM_EABI(HF)=y/\1/p" $CF | tr '[:upper:]' '[:lower:]')
      if [ ! -z "$HF" ]; then
        ARCH="${ARCH}_${HF}"
      fi
    fi

    if [ -z "$ARCH" ]; then
      ARCH=$(sed -rn "s/^BR2_(mips_[a-z]*)=y/\1/p" $CF)
    fi

    LIBC=$(sed -rn "s/^BR2_TOOLCHAIN_BUILDROOT_LIBC=\"(.*)\"/\1/p" $CF)

    SOC=$(echo $CF | cut -d _ -f 3)
    case $FMT in
      "list")
        echo $ARCH $GCC_VER $LIBC $VER $VENDOR $CF
        ;;
      "uniq")
        echo $ARCH $GCC_VER $LIBC $VER
        ;;
      "board")
        echo ${ARCH}-gcc${GCC_VER}-${LIBC}-${VER}
        ;;
    esac
  fi
}

if [ $# -eq 0 ]; then
  $0 list | sort
else
  if [ "$1" == "board" ]; then
    toolchain_by_config $2 $1
  else
    for D in br-ext-chip-*; do
      for CF in $D/configs/*; do
        toolchain_by_config $CF $1
      done
    done
  fi
fi
