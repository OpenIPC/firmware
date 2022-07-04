#!/bin/bash
#
# Please check BR2_ROOTFS_POST_BUILD_SCRIPT option in config
#

date "+OPENIPC_VERSION=2.2.%m.%d" >>${TARGET_DIR}/usr/lib/os-release
date "+GITHUB_VERSION=\"${BRANCH_NAME}+${GIT_HASH}, %Y-%m-%d\"" >>${TARGET_DIR}/usr/lib/os-release
if grep -q fpv_defconfig ${BR2_CONFIG}; then
    echo "BUILD_OPTION=fpv" >>${TARGET_DIR}/usr/lib/os-release
elif grep -q ultimate_defconfig ${BR2_CONFIG}; then
    echo "BUILD_OPTION=ultimate" >>${TARGET_DIR}/usr/lib/os-release
else
    echo "BUILD_OPTION=lite" >>${TARGET_DIR}/usr/lib/os-release
fi
#
echo 'Note: BR2_TOOLCHAIN_BUILDROOT_LIBC="glibc"'
#
# Comment out this line if you want to save the libraries
# rm -f ${TARGET_DIR}/usr/lib/libstdc++*
#
