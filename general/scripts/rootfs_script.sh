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

# Comments are worth writing and worth keeping in git; they are not worth
# flashing. sysupgrade alone had grown to 52KB, 57% of it comment, and on
# 2026-08-18 it pushed hi3519v101_lite 4KB past its 5120KB rootfs cap -- a board
# that had been sitting at exactly 5120/5120 for some time. Stripping here buys
# 16KB back on that image and ~24KB across all shipped scripts.
#
# Runs LAST, so the late overlays and hooks above are covered too. Discovery is
# by shebang, matching test_shell_parse.sh -- including its one exception,
# /etc/profile, which the login shell sources and which carries no shebang.
STRIPPER="${BR2_EXTERNAL_GENERAL_PATH}/scripts/strip-shell-comments.awk"
if [ -f "${STRIPPER}" ]; then
	STRIP_TMP=$(mktemp)
	# -type f skips the busybox applet symlinks; writing through `cat` rather
	# than `mv` keeps each file's own mode and inode.
	find "${TARGET_DIR}" -type f | while IFS= read -r script; do
		# Weed out binaries before reading a line of one: a rootfs is mostly
		# ELF, and their NUL bytes make the shebang test below warn per file.
		grep -Iq . "${script}" 2>/dev/null || continue

		case "$(head -1 "${script}" 2>/dev/null)" in
			'#!'*sh*) ;;
			*) [ "${script}" = "${TARGET_DIR}/etc/profile" ] || continue ;;
		esac

		awk -f "${STRIPPER}" "${script}" > "${STRIP_TMP}" 2>/dev/null || continue
		# A truncated result means awk gave up half way; keep the original.
		[ -s "${STRIP_TMP}" ] || continue
		cat "${STRIP_TMP}" > "${script}"
	done
	rm -f "${STRIP_TMP}"
fi
