#!/bin/bash

D=$(date +%y.%m.%d)
R=${TARGET_DIR}/usr/lib/os-release
echo OPENIPC_VERSION=${D:0:1}.${D:1} >> ${R}
date +GITHUB_VERSION="\"${GIT_BRANCH}+${GIT_HASH}, %Y-%m-%d"\" >> ${R}
date +TIME_STAMP=%s >> ${R}
echo BUILD_OPTION=${OPENIPC_RELEASE} >> ${R}

echo --- BR2_TOOLCHAIN_BUILDROOT_LIBC: ${BR2_EXTERNAL_LIBC}
rm -f ${TARGET_DIR}/usr/bin/gdbserver

if [ ${BR2_EXTERNAL_LIBC} != "glibc" ] && \
  ! grep -q ^BR2_PACKAGE_WIFIBROADCAST=y ${BR2_CONFIG} && \
  ! grep -q ^BR2_PACKAGE_LIBV4L=y ${BR2_CONFIG} && \
  ! grep -q ^BR2_PACKAGE_MAVLINK_ROUTER=y ${BR2_CONFIG} && \
  ! grep -q ^BR2_PACKAGE_INGENIC_OSDRV_T30=y ${BR2_CONFIG}; then
    rm -f ${TARGET_DIR}/usr/lib/libstdc++*
fi

if [ ${BR2_EXTERNAL_LIBC} = "musl" ]; then
  exlist=${BR2_EXTERNAL_SCRIPTS}/excludes/${OPENIPC_MODEL}_${OPENIPC_RELEASE}.list
  if [ -f ${exlist} ]; then
    for e in $(cat ${exlist}); do
      rm -f ${TARGET_DIR}${e}
    done
  fi

  ln -sf /lib/libc.so ${TARGET_DIR}/lib/ld-uClibc.so.0
  ln -sf ../../lib/libc.so ${TARGET_DIR}/usr/bin/ldd
fi
