#!/bin/sh
#
# usb0 comes and goes with the cable, long after S40network has run: Android
# switches tethering off whenever the cable is pulled, so the phone is
# re-plugged and re-enabled at any time. Bring the interface up when it appears
# and forget it when it goes. Without the ifdown, ifupdown still records the
# vanished interface as configured, and the next plug got "ifup: interface usb0
# already configured" and no address.

# Both halves under one lock: the cable can be pulled while udhcpc is still in
# its foreground retries, and an ifdown that finished first would leave the
# late ifup's state behind for the next plug to trip over. The lock orders
# nothing, though -- a pull within a second of the plug took it first -- so
# the add looks again once it holds it.
lock=/var/run/usbnet.$MDEV.lock

case "$ACTION" in
	add)
		# Only the host side of a tether. A USB gadget's usb0 -- the camera
		# being the device -- has no DHCP server across the cable, and
		# udhcpc's leasefail would hand it the camera's ipaddr.
		case "$(readlink "/sys/class/net/$MDEV/device/driver")" in
			*/rndis_host | */cdc_ether | */cdc_ncm) ;;
			*) exit 0 ;;
		esac
		# In the background: mdev waits for its helper, and udhcpc tries
		# for seconds before it daemonizes. The daemon must not take the
		# lock with it, hence the 9>&-. And ifup must not be the last
		# command here: ash execs that one in place of the subshell, the
		# 9>&- then drops the lock for the whole ifup, and a pull during
		# DHCP left usb0 recorded as configured every time.
		(
			flock 9
			[ -e "/sys/class/net/$MDEV" ] || exit 0
			# A new interface: whatever ifupdown holds for the name
			# is left over from the last one.
			ifdown -f "$MDEV"
			ifup "$MDEV" 9>&- || logger -t network "$MDEV: ifup failed"
		) 9>"$lock" </dev/null >/dev/null 2>&1 &
		;;
	remove)
		(
			flock 9
			ifdown -f "$MDEV"
		) 9>"$lock" >/dev/null 2>&1
		;;
esac
