#!/bin/sh
# xiongmai-osdrv-xm530 post-build hook.
#
# The vendor SD card driver (Arasan SDHC, module name sdio0_sd) ships in the
# image but nothing ever loads it: it is not listed in /etc/modules, so
# S35modules never modprobes it and /dev/mmcblk0 does not appear even with a
# card in the slot. mdev's automount helper and majestic's record path expect
# the card at /dev/mmcblk0p1.
#
# general/overlay/etc/modules is shared by every SoC, so the entry is appended
# here, only for the images that select this package. The hook must run after
# the rootfs overlay is applied, hence late-post-build-hooks.list rather than
# the package's INSTALL_TARGET_CMDS.
#
# Hot-plug is out of scope: the driver loads with detect=0, so sdio0_powerup()
# leaves GPIO49 (sdio0_detect) unmuxed and need_detect/need_poll are both 0.
# A card present at boot enumerates; one inserted afterwards is not detected.
# /etc/modules takes arguments, so "sdio0_sd detect=1" is available if that
# should change.
set -eu

TARGET_DIR="${1:?target dir required}"

MODULES="${TARGET_DIR}/etc/modules"
if [ ! -f "${MODULES}" ]; then
	echo "xiongmai-osdrv-xm530: ${MODULES} not found, cannot append sdio0_sd" >&2
	exit 1
fi

if ! grep -qx "sdio0_sd" "${MODULES}"; then
	printf 'sdio0_sd\n' >> "${MODULES}"
fi
