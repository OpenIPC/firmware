#!/bin/bash
DATE=$(date +%y.%m.%d)
FILE=${TARGET_DIR}/usr/lib/os-release

echo OPENIPC_VERSION=${DATE:0:1}.${DATE:1} >> ${FILE}
date +GITHUB_VERSION="\"${GIT_BRANCH-local}+${GIT_HASH-build}, %Y-%m-%d"\" >> ${FILE}
echo BUILD_OPTION=${OPENIPC_VARIANT} >> ${FILE}
date +TIME_STAMP=%s >> ${FILE}

cd ${BINARIES_DIR}
ext4_pos="$(fdisk  -l sysimage-sdcard.img | grep sysimage-sdcard.img2 | cut -d ' ' -f2)"
dd if=rootfs.ext4  of=sysimage-sdcard.img seek=${ext4_pos} conv=notrunc
gzip -k -f sysimage-sdcard.img
chmod a+r sysimage-sdcard.img.gz
cd -
echo -e "k230 image is \033[31m ${BINARIES_DIR}/sysimage-sdcard.img.gz \033[0m"
