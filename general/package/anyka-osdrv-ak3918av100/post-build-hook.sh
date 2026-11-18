#!/bin/sh
# anyka-osdrv-ak3918av100 post-build hook.
#
# The SD controller, Ethernet MAC and USB host drivers of these parts exist
# only as prebuilt modules (ak_mci, ak_eth, ak_hcd); the kernel has none built
# in. load_anyka runs from S70vendor, after S40network has already tried to
# bring eth0 up and load the USB Wi-Fi driver, so the ones the image carries
# are listed in /etc/modules for S35modules to load first.
#
# The kernel builds the MMC block driver as a module too, and mdev does not
# load modules by alias, so mmc_block goes in after ak_mci: without it a
# card is found but no /dev/mmcblk0 appears. The camera's stock firmware
# ships and loads mmc_block.ko the same way.
set -eu

TARGET_DIR="${1:?target dir required}"

MODULES="${TARGET_DIR}/etc/modules"
if [ ! -f "${MODULES}" ]; then
	echo "anyka-osdrv-ak3918av100: ${MODULES} not found, cannot append its modules" >&2
	exit 1
fi

for mod in ak_mci mmc_block ak_eth ak_hcd; do
	ls "${TARGET_DIR}"/lib/modules/*/anyka/"${mod}.ko" \
		"${TARGET_DIR}"/lib/modules/*/kernel/drivers/mmc/card/"${mod}.ko" \
		2>/dev/null | grep -q . || continue
	if ! grep -qx "${mod}" "${MODULES}"; then
		printf '%s\n' "${mod}" >> "${MODULES}"
	fi
done
