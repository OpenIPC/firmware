#!/usr/bin/env bash

shopt -s nullglob

if [ $# -eq 0 ]; then
  $0 1 | sort | uniq | awk '{printf("%10s\t%s %s\n", $2, $3, $4)}'
else
  for D in br-ext-chip-*; do
    for CF in $D/configs/*; do
      BR2_VER=$(grep BR2_DEFAULT_KERNEL_VERSION $CF)
      if [ ! -z $BR2_VER ]; then
        VENDOR=$(echo $CF | cut -d - -f 4 | cut -d / -f 1 | sed -r 's/\<./\U&/g')
        VER=$(echo $BR2_VER | cut -d \" -f 2)
        HASH=$(echo $VER | awk -F . '{print $1*1000000+$2*1000+$3}' )
        SOC=$(echo $CF | cut -d _ -f 3)
        echo $HASH $VER $VENDOR $SOC
      fi
    done
  done
fi
