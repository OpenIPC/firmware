#!/usr/bin/env bash

FROM=7_X
TO=8_X

FILES=$(grep -rl BR2_GCC_VERSION_${FROM} br-ext-chip-*)

for f in $FILES
do
  	echo "Processing $f"
    sed -i "s/BR2_GCC_VERSION_${FROM}/BR2_GCC_VERSION_${TO}/" $f
done
