#!/bin/sh
# anyka-osdrv-ak3918av130 post-build hook.
#
# The AV130's Ethernet MAC, SD controller and USB host drivers exist only as
# SDK modules (ak_eth, ak_mci, ak_hcd); the kernel has none built in.
# load_anyka runs from S70vendor, after S40network has already tried to bring
# eth0 up and load the USB Wi-Fi driver, so these are listed in /etc/modules
# for S35modules to load first. Without ak_hcd there, a USB radio -- the
# Yoton WS03's only network, OpenIPC/firmware#2962 -- was not on the bus yet
# when S40network looked for it. modprobe brings in usbcore with it.
#
# general/overlay/etc/modules is shared by every SoC, so the entries are
# appended here, only for the images that select this package. The hook must
# run after the rootfs overlay is applied, hence late-post-build-hooks.list
# rather than the package's INSTALL_TARGET_CMDS.
set -eu

TARGET_DIR="${1:?target dir required}"

MODULES="${TARGET_DIR}/etc/modules"
if [ ! -f "${MODULES}" ]; then
	echo "anyka-osdrv-ak3918av130: ${MODULES} not found, cannot append ak_eth/ak_mci/ak_hcd" >&2
	exit 1
fi

for mod in ak_eth ak_mci ak_hcd; do
	if ! grep -qx "${mod}" "${MODULES}"; then
		printf '%s\n' "${mod}" >> "${MODULES}"
	fi
done
