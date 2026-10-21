#!/bin/sh
#
# usb0 comes and goes with the cable, long after S40network has run: Android
# switches tethering off whenever the cable is pulled, so the phone is
# re-plugged and re-enabled at any time. Bring the interface up when it appears
# and forget it when it goes. Without the ifdown, ifupdown still records the
# vanished interface as configured, and the next plug got "ifup: interface usb0
# already configured" and no address.

case "$ACTION" in
	add)
		# Only the host side of a tether. A USB gadget's usb0 -- the camera
		# being the device -- has no DHCP server across the cable, and
		# udhcpc's leasefail would hand it the camera's ipaddr.
		case "$(readlink "/sys/class/net/$MDEV/device/driver")" in
			*/rndis_host | */cdc_ether | */cdc_ncm) ;;
			*) exit 0 ;;
		esac
		# mdev waits for its helper, and udhcpc tries for seconds before
		# it backgrounds.
		ifup "$MDEV" </dev/null >/dev/null 2>&1 &
		;;
	remove)
		ifdown -f "$MDEV" >/dev/null 2>&1
		;;
esac
