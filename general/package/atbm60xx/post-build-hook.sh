#!/bin/sh
# atbm60xx post-build hook.
#
# The atbm603x-xm530-usb bring-up case in /etc/wireless/usb is board-specific
# and only works where the xiongmai vendor OTG/PDN stack and the 603x USB
# driver module are both in the image. It is spliced in here, only for the
# images that select this package, instead of being shipped through the
# shared overlay. Same rationale as the xiongmai-osdrv-xm530 sdio0_sd hook:
# general/overlay is shared by every SoC. The hook must run after the rootfs
# overlay is applied, hence late-post-build-hooks.list.
#
# The case must land before the file's final "exit 1" or it is dead code.
set -eu

TARGET_DIR="${1:?target dir required}"
USB="${TARGET_DIR}/etc/wireless/usb"

if [ ! -f "${USB}" ]; then
	echo "atbm60xx: ${USB} not found, cannot splice wireless case" >&2
	exit 1
fi

# Only meaningful where the case can actually work: the xiongmai vendor
# wifi_pdn module and the 603x USB driver are both present in this image.
[ -n "$(find "${TARGET_DIR}/lib/modules" -path '*/xiongmai/wifi_pdn.ko' -print -quit)" ] || exit 0
[ -n "$(find "${TARGET_DIR}/lib/modules" -name atbm603x_wifi_usb.ko -print -quit)" ] || exit 0

# Idempotent: skip if the case is already present.
grep -q 'atbm603x-xm530-usb' "${USB}" && exit 0

# Exactly one final "exit 1" to splice before.
[ "$(grep -c '^exit 1$' "${USB}")" -eq 1 ] || {
	echo "atbm60xx: expected exactly one 'exit 1' in ${USB}" >&2
	exit 1
}

CASE_FILE=$(mktemp)
trap 'rm -f "${CASE_FILE}"' EXIT
cat > "${CASE_FILE}" <<'EOF'

# XM530 AltoBeam ATBM6032 (USB 007a:8888); PDN gpio from the wifipdn env
# (set per board, e.g. by the builder profile's customizer)
if [ "$1" = "atbm603x-xm530-usb" ]; then
	modprobe dwc_otg
	pdn=$(fw_printenv -n wifipdn)
	[ -n "$pdn" ] && modprobe wifi_pdn value="$pdn"
	modprobe atbm603x_wifi_usb
	exit 0
fi
EOF

awk -v cf="${CASE_FILE}" '
	/^exit 1$/ { while ((getline line < cf) > 0) print line; close(cf) }
	{ print }
' "${USB}" > "${USB}.new"
chmod --reference="${USB}" "${USB}.new"
mv "${USB}.new" "${USB}"
