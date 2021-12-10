#!/bin/bash
#
# Please check BR2_ROOTFS_POST_BUILD_SCRIPT option in config
#

GITHUB_VERSION="${BRANCH_NAME}+${GIT_HASH}" 
RELEASE_DATE=$(date "+%y.%m.%d") 
echo "OPENIPC_VERSION=${GITHUB_VERSION}, 2.${RELEASE_DATE:1}" >>${TARGET_DIR}/usr/lib/os-release
#
echo 'Note: BR2_TOOLCHAIN_BUILDROOT_LIBC="uclibc"'
#
