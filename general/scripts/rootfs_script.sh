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

# The image ships no majestic.yaml: majestic runs on its own defaults until a
# setting is saved. The package stopped installing one, but an output directory
# built before that still holds it, and Buildroot does not reinstall a package
# whose recipe changed, so it is removed here on every build.
rm -f ${TARGET_DIR}/etc/majestic.yaml

CONF="USES_GLIBC=y|OSDRV_T30=y|OSDRV_V85X=y|LIBV4L=y|MAVLINK_ROUTER=y|RUBYFPV=y|ONYXFPV=y|WIFIBROADCAST=y|WIFIBROADCAST_NG=y|AUDIO_PROCESSING_OPENIPC=y"
if ! grep -qP ${CONF} ${BR2_CONFIG}; then
	rm -f ${TARGET_DIR}/usr/lib/libstdc++*
fi

if grep -q "USES_MUSL=y" ${BR2_CONFIG}; then
	ln -sf libc.so ${TARGET_DIR}/lib/ld-uClibc.so.0
	ln -sf ../../lib/libc.so ${TARGET_DIR}/usr/bin/ldd

	# The external toolchain copies libgcc_s and libatomic into every image
	# whether anything links them or not: 36KB of squashfs on hi3516ev300,
	# which is what tipped its lite board over the cap on 2026-09-25. musl
	# never loads libgcc_s itself -- uClibc and glibc do, for pthread_cancel,
	# so this stays inside the musl branch. The test is the name appearing
	# anywhere in the target, which covers a NEEDED entry and a dlopen() by
	# literal name alike, and keeps them for any board that ships C++.
	for lib in libgcc_s libatomic; do
		if ! grep -rqaF -D skip --exclude="${lib}.so*" "${lib}.so" ${TARGET_DIR}; then
			rm -f ${TARGET_DIR}/lib/${lib}.so* ${TARGET_DIR}/usr/lib/${lib}.so*
		fi
	done
fi

# depmod writes a binary index beside every text one, plus
# modules.builtin.modinfo, for kmod. Every board here runs busybox modprobe,
# which reads modules.dep, modules.alias, modules.symbols and modules.builtin as
# text and never opens the rest: 52KB on hi3516cv6xx, 12KB of squashfs, enough
# to bring its lite board back under the cap. Kept wherever kmod is installed.
if [ -z "$(find ${TARGET_DIR}/bin ${TARGET_DIR}/sbin ${TARGET_DIR}/usr/bin ${TARGET_DIR}/usr/sbin -name kmod -type f 2>/dev/null)" ]; then
	rm -f ${TARGET_DIR}/lib/modules/*/modules.*.bin ${TARGET_DIR}/lib/modules/*/modules.builtin.modinfo
fi

LIST="${BR2_EXTERNAL_GENERAL_PATH}/scripts/excludes/${OPENIPC_SOC_MODEL}_${OPENIPC_VARIANT}.list"
if [ -f "${LIST}" ]; then
	# These lists name files by hand, so they go stale in one direction without
	# anything saying so: a package renames or drops a sensor blob and the entry
	# that used to prune it silently prunes nothing, while the board keeps paying
	# for whatever replaced it. OpenIPC/builder's hi3518ev200_lite list names 25
	# sensor .so files where the package now ships 17. The old form was a single
	# `xargs -a ... rm -f`, which cannot tell the two cases apart -- and fed its
	# `#` separator lines to rm as literal paths besides.
	#
	# Report, never fail: an image that ships a few kB it meant to drop is a
	# size problem to look at, not a reason to break the build.
	stale=0
	total=0
	while IFS= read -r entry || [ -n "${entry}" ]; do
		case "${entry}" in
			''|\#*) continue ;;
		esac
		total=$((total + 1))
		if [ -e "${TARGET_DIR}${entry}" ] || [ -L "${TARGET_DIR}${entry}" ]; then
			rm -f "${TARGET_DIR}${entry}"
		else
			stale=$((stale + 1))
			echo "excludes: ${entry} matched no file"
		fi
	done < "${LIST}"
	if [ ${stale} -gt 0 ]; then
		echo "excludes: ${stale} of ${total} entries in ${LIST##*/} matched no file"
	fi
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

# NAND FIT: a board whose NAND image carries the kernel as a FIT in its
# `kernel` UBI volume ships board/<family>/nand-fit.its. ubinize packs the
# volumes right after this script and before post-image, so the FIT has to
# exist by now. The kernel and its DTB come straight from the kernel tree --
# BINARIES_DIR only gets the uImage, which has the DTB appended and is what the
# NOR image still boots.
NAND_FIT_ITS="${BR2_EXTERNAL_GENERAL_PATH}/../br-ext-chip-${OPENIPC_SOC_VENDOR}/board/${OPENIPC_SOC_FAMILY}/nand-fit.its"
# One built for another board in a reused output directory must not ride along:
# repack packs whatever fitImage it finds. (cv6xx makes its own in post-image,
# which runs after this.)
rm -f "${BINARIES_DIR}/fitImage"
if [ -f "${NAND_FIT_ITS}" ] && grep -q "^BR2_TARGET_ROOTFS_UBI=y" "${BR2_CONFIG}"; then
	KBOOT=$(ls -d "${BUILD_DIR}"/linux-*/arch/arm/boot 2>/dev/null | grep -v headers | head -1)
	FIT_DIR="${BINARIES_DIR}/nand-fit"
	rm -rf "${FIT_DIR}" && mkdir -p "${FIT_DIR}" || exit 1
	# @SOC@ is the SoC the FIT is stamped with (sysupgrade's fit_soc reads it);
	# @DTB@ is for a family .its whose models each build their own
	# <model>-demb.dtb (hi3516ev200 family). An .its naming its DTB outright
	# has no @DTB@ and is copied as it is.
	sed -e "s/@SOC@/${OPENIPC_SOC_MODEL}/" -e "s/@DTB@/${OPENIPC_SOC_MODEL}-demb.dtb/g" \
		"${NAND_FIT_ITS}" > "${FIT_DIR}/nand-fit.its" || exit 1
	cp "${KBOOT}/zImage" "${FIT_DIR}/" || { echo "NAND FIT: no zImage in ${KBOOT}" >&2; exit 1; }
	# Every DTB the stamped .its names, from the kernel's dts output.
	for dtb in $(grep -o '/incbin/("[^"]*\.dtb")' "${FIT_DIR}/nand-fit.its" | sed 's/.*("\(.*\)")/\1/'); do
		cp "${KBOOT}/dts/${dtb}" "${FIT_DIR}/" || { echo "NAND FIT: no ${dtb} in ${KBOOT}/dts" >&2; exit 1; }
	done
	"${HOST_DIR}/bin/mkimage" -f "${FIT_DIR}/nand-fit.its" "${BINARIES_DIR}/fitImage" || exit 1
	rm -rf "${FIT_DIR}"
fi

# Root's login shell on an unclaimed camera is /usr/sbin/openipc-claim (see
# overlay/etc/passwd), and dropbear checks a login shell against /etc/shells
# through getusershell() BEFORE it ever runs -- an unlisted shell is rejected at
# authentication with "Permission denied", so the gate would never get to run
# and, worse, could never disable itself either: the self-heal that puts /bin/sh
# back happens at login, and there is no login. Verified on hi3516ev300, where
# key auth stopped working the moment the shell changed.
#
# Appended here rather than shipped as overlay/etc/shells because the file is
# built up by TARGET_FINALIZE_HOOKS -- busybox adds /bin/ash, skeleton-init
# adds /bin/sh -- and the overlay is rsynced over the target AFTER those hooks
# have run. An overlay copy would replace their work with a hardcoded list that
# goes quietly wrong the next time buildroot changes what it registers. The
# post-build script runs after both, so appending composes with whatever they
# decided. Same grep guard buildroot's own hooks use, so a re-run adds nothing.
CLAIM_SHELL=/usr/sbin/openipc-claim
if [ -x "${TARGET_DIR}${CLAIM_SHELL}" ]; then
	grep -qsE "^${CLAIM_SHELL}\$" "${TARGET_DIR}/etc/shells" \
		|| echo "${CLAIM_SHELL}" >> "${TARGET_DIR}/etc/shells"
fi

# Mozilla's whole store is 121 roots and ~100KB of squashfs; most of it is
# national and regional roots a camera's outbound HTTPS never meets. Lite keeps
# the operators named in ca-bundle-lite.keep, ~60KB less (#2508). A bundle that
# lost GitHub's or Let's Encrypt's root would only show up once sysupgrade had
# nowhere left to fetch from, so a keep-list that has gone stale fails the build
# here instead.
CA_BUNDLE="${TARGET_DIR}/etc/ssl/certs/ca-certificates.crt"
if [ "${OPENIPC_VARIANT}" = "lite" ] && [ -f "${CA_BUNDLE}" ]; then
	python3 "${BR2_EXTERNAL_GENERAL_PATH}/scripts/filter-ca-bundle.py" \
		"${BR2_EXTERNAL_GENERAL_PATH}/scripts/ca-bundle-lite.keep" "${CA_BUNDLE}" || exit 1
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
	STRIP_ERR=$(mktemp)
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

		# The redirection truncates ${script} before cat writes a byte, so a
		# failure here -- ENOSPC is the realistic one, on a runner that has just
		# built a rootfs -- leaves a half-written or empty script in the image.
		# That is the exact thing this pass must not do: an empty S40network or
		# load_hisilicon still builds green and bricks the camera quietly. There
		# is no original left to restore by then, so fail the build instead.
		if ! cat "${STRIP_TMP}" > "${script}"; then
			echo "rootfs_script: failed to write stripped ${script}" >&2
			echo failed > "${STRIP_ERR}"
			break
		fi
	done

	# `find | while` runs the loop in a subshell, so the failure comes back
	# through the file rather than through its exit status.
	if [ -s "${STRIP_ERR}" ]; then
		rm -f "${STRIP_TMP}" "${STRIP_ERR}"
		exit 1
	fi
	rm -f "${STRIP_TMP}" "${STRIP_ERR}"
fi
