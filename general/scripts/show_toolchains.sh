#!/bin/bash
if [ -z "$1" ]; then
	echo "Configuration file missing"
	exit 0
fi

LIST="arm11.+=y|cortex.+=y|mips_xburst=y"
SOC=$(grep -E "${LIST}" "$1" | sed -rn "s/BR2_(.*)=y/\1/p")
THUMB=$(grep -q "THUMB2=y" "$1" && echo _thumb2)
FLOAT=$(grep -qE "VFPV3=y|VFPV4=y" "$1" && ! grep -q "ARM_EABI=y" "$1" && echo _hf)
LINUX=$(grep -oP "HEADERS.+\K\d+.\d+" "$1")

if grep -q "MUSL=y" "$1"; then
	LIB=musl
elif grep -q "UCLIBC=y" "$1"; then
	LIB=uclibc
fi

echo "${SOC:-arm926t}${THUMB}${FLOAT}-gcc13-${LIB:-glibc}-${LINUX}"
