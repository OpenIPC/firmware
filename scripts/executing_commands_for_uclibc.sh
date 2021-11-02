#!/bin/bash
#
# Please check BR2_ROOTFS_POST_BUILD_SCRIPT option in config
#

echo 'Note: BR2_TOOLCHAIN_BUILDROOT_LIBC="uclibc"'
#
date "+OPENIPC_VERSION=2.1.%m.%d" >${TARGET_DIR}/etc/openipc_version
#
