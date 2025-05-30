#!/bin/bash
DATE=$(date +%y.%m.%d)
FILE=${TARGET_DIR}/usr/lib/os-release

echo OPENIPC_VERSION=${DATE:0:1}.${DATE:1} >> ${FILE}
date +GITHUB_VERSION="\"${GIT_BRANCH-local}+${GIT_HASH-build}, %Y-%m-%d"\" >> ${FILE}
echo BUILD_OPTION=${OPENIPC_VARIANT} >> ${FILE}
date +TIME_STAMP=%s >> ${FILE}


k230_openipc_img_replace_rootfs()
{
    cd ${BINARIES_DIR}
    ext4_pos="$(fdisk  -l sysimage-sdcard.img | grep sysimage-sdcard.img2 | cut -d ' ' -f2)"
    dd if=rootfs.ext4  of=sysimage-sdcard.img seek=${ext4_pos} conv=notrunc
    gzip -k -f sysimage-sdcard.img
    chmod a+r sysimage-sdcard.img.gz
    cd -
    echo -e "k230 image is \033[31m ${BINARIES_DIR}/sysimage-sdcard.img.gz \033[0m"
}
k230_openipc_img_env_replace()
{
    cd ${BINARIES_DIR}
    mkenvimage  -s 0x10000 -o env.env  ${BR2_EXTERNAL}/package/canaan_k230_sdk/k230_env.env

    dd if=env.env  of=sysimage-sdcard.img seek=3840 conv=notrunc
    cd -;
}

k230_openipc_img_env_replace
k230_openipc_img_replace_rootfs
