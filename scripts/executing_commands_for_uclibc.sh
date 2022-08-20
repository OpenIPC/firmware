#!/bin/bash
#
# Please check BR2_ROOTFS_POST_BUILD_SCRIPT option in config
#

D=$(date "+%y.%m.%d")
echo "OPENIPC_VERSION=${D:0:1}.${D:1}" >>${TARGET_DIR}/usr/lib/os-release
date "+GITHUB_VERSION=\"${BRANCH}+${GIT_HASH}, %Y-%m-%d\"" >>${TARGET_DIR}/usr/lib/os-release
if grep -q fpv_defconfig ${BR2_CONFIG}; then
    echo "BUILD_OPTION=fpv" >>${TARGET_DIR}/usr/lib/os-release
elif grep -q ultimate_defconfig ${BR2_CONFIG}; then
    echo "BUILD_OPTION=ultimate" >>${TARGET_DIR}/usr/lib/os-release
else
    echo "BUILD_OPTION=lite" >>${TARGET_DIR}/usr/lib/os-release
fi
#
echo 'Note: BR2_TOOLCHAIN_BUILDROOT_LIBC="uclibc"'
#

if ! grep -q ^BR2_PACKAGE_WIFIBROADCAST=y ${BR2_CONFIG}; then
	rm -f ${TARGET_DIR}/usr/lib/libstdc++*
fi

rm -f ${TARGET_DIR}/usr/bin/gdbserver
