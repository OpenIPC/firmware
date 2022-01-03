#!/bin/bash
#
# Please check BR2_ROOTFS_POST_BUILD_SCRIPT option in config
#

date "+OPENIPC_VERSION=2.2.%m.%d" >>${TARGET_DIR}/usr/lib/os-release
date "+GITHUB_VERSION=\"${BRANCH_NAME}+${GIT_HASH}, %Y-%m-%d\"" >>${TARGET_DIR}/usr/lib/os-release
echo "BUILD_OPTION=lite" >>${TARGET_DIR}/usr/lib/os-release
#
echo 'Note: BR2_TOOLCHAIN_BUILDROOT_LIBC="musl"'
#
ln -sfv /lib/libc.so ${TARGET_DIR}/lib/ld-uClibc.so.0
ln -sfv ../../lib/libc.so ${TARGET_DIR}/usr/bin/ldd
#
# Comment out this line if you want to save the libraries
rm -f ${TARGET_DIR}/usr/lib/libstdc++*
#
