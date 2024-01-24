#!/bin/bash
INDIR=br-ext-chip-hisilicon/board/hi3516ev200/kernel
OUTDIR=br-ext-chip-goke/board/gk7205v200/kernel

function map_soc() {
	case $1 in
		hi3516ev200) NEW_SOC=gk7205v200 ;;
		hi3516dv200) NEW_SOC=gk7605v100 ;;
		hi3516ev300) NEW_SOC=gk7205v300 ;;
		hi3518ev300) NEW_SOC=gk7202v300 ;;
	esac
}

pushd $(git rev-parse --show-toplevel) > /dev/null
for FILENAME in ${INDIR}/*.config; do
	OLD_CONFIG=$(basename $FILENAME)
	OLD_SOC=$(echo $OLD_CONFIG | cut -f 1 -d .)
	map_soc $OLD_SOC
	NEW_CONFIG=$(echo $OLD_CONFIG | sed "s/$OLD_SOC/$NEW_SOC/")
	general/scripts/clone_hisi2goke.sh $INDIR/$OLD_CONFIG $OUTDIR/$NEW_CONFIG
	git add $OUTDIR/$NEW_CONFIG
done
popd > /dev/null
