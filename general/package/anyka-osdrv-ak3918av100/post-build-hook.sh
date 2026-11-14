#!/bin/sh
# anyka-osdrv-ak3918av100 post-build hook.
#
# The SD controller and Ethernet MAC drivers of these parts exist only as
# prebuilt modules (ak_mci, ak_eth); the kernel has neither built in.
# load_anyka runs from S70vendor, after S40network has already tried to bring
# eth0 up, so the ones the image carries are listed in /etc/modules for
# S35modules to load first, as anyka-osdrv-ak3918av130 does.
set -eu

TARGET_DIR="${1:?target dir required}"

MODULES="${TARGET_DIR}/etc/modules"
if [ ! -f "${MODULES}" ]; then
	echo "anyka-osdrv-ak3918av100: ${MODULES} not found, cannot append ak_mci/ak_eth" >&2
	exit 1
fi

for mod in ak_mci ak_eth; do
	ls "${TARGET_DIR}"/lib/modules/*/anyka/"${mod}.ko" >/dev/null 2>&1 || continue
	if ! grep -qx "${mod}" "${MODULES}"; then
		printf '%s\n' "${mod}" >> "${MODULES}"
	fi
done
