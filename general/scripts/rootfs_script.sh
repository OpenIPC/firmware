#!/bin/bash
DATE=$(date +%y.%m.%d)
FILE=${TARGET_DIR}/usr/lib/os-release
LATE_OVERLAY_LIST="${BR2_EXTERNAL_GENERAL_PATH}/scripts/late-overlays.list"
LATE_POST_BUILD_HOOKS="${BR2_EXTERNAL_GENERAL_PATH}/scripts/late-post-build-hooks.list"

echo OPENIPC_VERSION=${DATE:0:1}.${DATE:1} >> ${FILE}
date +GITHUB_VERSION="\"${GIT_BRANCH-local}+${GIT_HASH-build}, %Y-%m-%d"\" >> ${FILE}
echo BUILD_OPTION=${OPENIPC_VARIANT} >> ${FILE}
echo BUILD_ID=${BUILD_ID:-local-$(date -u +%Y%m%d)-${GIT_HASH-build}} >> ${FILE}
echo BUILD_SHA=${BUILD_SHA:-${GIT_HASH-build}} >> ${FILE}
echo BUILD_PLATFORM=${BUILD_PLATFORM:-${OPENIPC_SOC_MODEL}_${OPENIPC_VARIANT}} >> ${FILE}
date +TIME_STAMP=%s >> ${FILE}

CONF="USES_GLIBC=y|OSDRV_T30=y|OSDRV_V85X=y|LIBV4L=y|MAVLINK_ROUTER=y|RUBYFPV=y|ONYXFPV=y|WIFIBROADCAST=y|WIFIBROADCAST_NG=y|AUDIO_PROCESSING_OPENIPC=y"
if ! grep -qP ${CONF} ${BR2_CONFIG}; then
	rm -f ${TARGET_DIR}/usr/lib/libstdc++*
fi

if grep -q "USES_MUSL=y" ${BR2_CONFIG}; then
	ln -sf libc.so ${TARGET_DIR}/lib/ld-uClibc.so.0
	ln -sf ../../lib/libc.so ${TARGET_DIR}/usr/bin/ldd
fi

LIST="${BR2_EXTERNAL_GENERAL_PATH}/scripts/excludes/${OPENIPC_SOC_MODEL}_${OPENIPC_VARIANT}.list"
if [ -f ${LIST} ]; then
	xargs -a ${LIST} -I % rm -f ${TARGET_DIR}%
fi

if [ -f "${LATE_OVERLAY_LIST}" ]; then
	while IFS=: read -r symbol overlay_relpath; do
		[ -n "${symbol}" ] || continue
		case "${symbol}" in
			\#*) continue ;;
		esac

		if grep -q "^${symbol}=y" "${BR2_CONFIG}"; then
			overlay_dir="${BR2_EXTERNAL_GENERAL_PATH}/${overlay_relpath}"
			if [ -d "${overlay_dir}" ]; then
				rsync -a "${overlay_dir}/" "${TARGET_DIR}/"
			fi
		fi
	done < "${LATE_OVERLAY_LIST}"
fi

if [ -f "${LATE_POST_BUILD_HOOKS}" ]; then
	while IFS=: read -r symbol hook_relpath; do
		[ -n "${symbol}" ] || continue
		case "${symbol}" in
			\#*) continue ;;
		esac

		if grep -q "^${symbol}=y" "${BR2_CONFIG}"; then
			hook_script="${BR2_EXTERNAL_GENERAL_PATH}/${hook_relpath}"
			if [ -x "${hook_script}" ]; then
				"${hook_script}" "${TARGET_DIR}"
			fi
		fi
	done < "${LATE_POST_BUILD_HOOKS}"
fi
