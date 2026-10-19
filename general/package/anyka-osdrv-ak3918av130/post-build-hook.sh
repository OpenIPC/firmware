#!/bin/sh
# anyka-osdrv-ak3918av130 post-build hook.
#
# The AV130's Ethernet MAC and SD controller drivers exist only as SDK
# modules (ak_eth, ak_mci); the kernel has neither built in. load_anyka runs
# from S70vendor, after S40network has already tried to bring eth0 up, so
# these two are listed in /etc/modules for S35modules to load first.
#
# general/overlay/etc/modules is shared by every SoC, so the entries are
# appended here, only for the images that select this package. The hook must
# run after the rootfs overlay is applied, hence late-post-build-hooks.list
# rather than the package's INSTALL_TARGET_CMDS.
set -eu

TARGET_DIR="${1:?target dir required}"

MODULES="${TARGET_DIR}/etc/modules"
if [ ! -f "${MODULES}" ]; then
	echo "anyka-osdrv-ak3918av130: ${MODULES} not found, cannot append ak_eth/ak_mci" >&2
	exit 1
fi

for mod in ak_eth ak_mci; do
	if ! grep -qx "${mod}" "${MODULES}"; then
		printf '%s\n' "${mod}" >> "${MODULES}"
	fi
done
