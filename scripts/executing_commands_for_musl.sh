#!/bin/bash
#
# Please check BR2_ROOTFS_POST_BUILD_SCRIPT option in config
#

D=$(date "+%y.%m.%d")
echo "OPENIPC_VERSION=${D:0:1}.${D:1}" >>${TARGET_DIR}/usr/lib/os-release
date "+GITHUB_VERSION=\"${BRANCH}+${GIT_HASH}, %Y-%m-%d\"" >>${TARGET_DIR}/usr/lib/os-release
if grep -q fpv_defconfig ${BR2_CONFIG}; then
    echo "BUILD_OPTION=fpv" >>${TARGET_DIR}/usr/lib/os-release
elif grep -q lte_defconfig ${BR2_CONFIG}; then
    echo "BUILD_OPTION=lte" >>${TARGET_DIR}/usr/lib/os-release
elif grep -q ultimate_defconfig ${BR2_CONFIG}; then
    echo "BUILD_OPTION=ultimate" >>${TARGET_DIR}/usr/lib/os-release
else
    echo "BUILD_OPTION=lite" >>${TARGET_DIR}/usr/lib/os-release
fi
#
echo 'Note: BR2_TOOLCHAIN_BUILDROOT_LIBC="musl"'
#
ln -sfv /lib/libc.so ${TARGET_DIR}/lib/ld-uClibc.so.0
ln -sfv ../../lib/libc.so ${TARGET_DIR}/usr/bin/ldd
#

if ! grep -q ^BR2_PACKAGE_WIFIBROADCAST=y ${BR2_CONFIG} && \
   ! grep -q ^BR2_PACKAGE_LIBV4L=y ${BR2_CONFIG} && \
   ! grep -q ^BR2_PACKAGE_MAVLINK_ROUTER=y ${BR2_CONFIG} && \
   ! grep -q ^BR2_PACKAGE_INGENIC_OSDRV_T30=y ${BR2_CONFIG} \
; then
	rm -f ${TARGET_DIR}/usr/lib/libstdc++*
fi

rm -f ${TARGET_DIR}/usr/bin/gdbserver

soc=$(grep defconfig ${BR2_CONFIG} | rev | cut -d "_" -f3- | cut -d "/" -f1 | rev)
release=$(cat ${TARGET_DIR}/usr/lib/os-release | grep BUILD_OPTION | cut -d "=" -f2)
exlist="${BASE_DIR}/../scripts/excludes/${soc}_${release}.list"

if [ -f ${exlist} ]; then
  for e in $(cat ${exlist}); do
	rm -f ${TARGET_DIR}${e}
  done
fi
