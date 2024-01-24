#!/bin/bash

if [ ! $# -eq 2 ]; then
	echo "Usage: $0 <hi3516dv200.generic.config> <gk7605v100.generic.config>"
fi

INPUT=$1
OUTPUT=$2

cp $INPUT $OUTPUT
sed -i 's/HISI_BVT/GOKE/g' $OUTPUT
sed -i 's/Hisilicon BVT/Goke/g' $OUTPUT
sed -i 's/HISILICON/GOKE/g' $OUTPUT
sed -i 's/HISI_XVP_USB2/GOKE_USBP2/g' $OUTPUT
sed -i 's/HISI/GOKE/g' $OUTPUT
sed -i 's/HI3516EV200/GK7205V200/g' $OUTPUT
sed -i 's/HI3516EV300/GK7205V300/g' $OUTPUT
sed -i 's/HI3518EV300/GK7202V300/g' $OUTPUT
sed -i 's/HI3516DV200/GK7605V100/g' $OUTPUT
sed -i 's/CONFIG_HI_/CONFIG_BSP_/g' $OUTPUT
sed -i 's/CONFIG_MTD_SPI_NAND_HIFMC100/CONFIG_MTD_SPI_NAND_FMC100/g' $OUTPUT
sed -i 's/HIBVT/GOKE/g' $OUTPUT
