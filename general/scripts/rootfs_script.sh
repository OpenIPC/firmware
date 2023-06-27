#!/bin/bash

DATE=$(date +%y.%m.%d)
FILE=${TARGET_DIR}/usr/lib/os-release

echo OPENIPC_VERSION=${DATE:0:1}.${DATE:1} >> ${FILE}
date +GITHUB_VERSION="\"${GIT_BRANCH-local}+${GIT_HASH-build}, %Y-%m-%d"\" >> ${FILE}
echo BUILD_OPTION=${OPENIPC_FLAVOR} >> ${FILE}
date +TIME_STAMP=%s >> ${FILE}

CONF="INGENIC_OSDRV_T30=y|LIBV4L=y|MAVLINK_ROUTER=y|WIFIBROADCAST=y|WEBRTC_AUDIO_PROCESSING=y"
if ! grep -q "USES_GLIBC" ${BR2_CONFIG} && ! grep -qP ${CONF} ${BR2_CONFIG}; then
  rm -f ${TARGET_DIR}/usr/lib/libstdc++*
fi

if grep -q "USES_MUSL" ${BR2_CONFIG}; then
  LIST=${BR2_EXTERNAL}/scripts/excludes/${OPENIPC_SOC_MODEL}_${OPENIPC_FLAVOR}.list
  test -e ${LIST} && xargs -a ${LIST} -I % rm -rf ${TARGET_DIR}/%

  ln -sf /lib/libc.so ${TARGET_DIR}/lib/ld-uClibc.so.0
  ln -sf ../../lib/libc.so ${TARGET_DIR}/usr/bin/ldd
fi
