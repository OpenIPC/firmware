#!/bin/sh
# The USB role, and nothing else.
#
# This camera has one USB port and it can be one of two things: a host, with a
# webcam plugged into it, or a device, pretending to be a webcam for a PC. That
# choice lives in the DWC3 controller and is remembered in /etc/usbmode. It is
# not a majestic setting and there is no majestic API for it, which is the whole
# reason this endpoint exists -- everything else the USB page reads or writes
# goes to majestic directly from the browser.
#
#   (no query)      what the port is doing right now
#   ?role=host      a webcam plugged into us
#   ?role=device    us, pretending to be one
#
# Switching does NOT restart majestic and does not reboot. majestic holds the
# video node in either role and the flip cannot happen while it does, so the
# sequence is: SIGQUIT to make it let go, flip, and let the caller's config
# write bring the pipeline back. SIGQUIT is the middle gear -- it releases the
# SDK and leaves the web server running, which matters more here than anywhere
# else, because the web server is what is serving the page that asked.

printf 'HTTP/1.1 200 OK\nContent-Type: application/json\nCache-Control: no-store\n\n'

# usr/sbin is where OpenIPC/firmware installs it. usr/bin is where the
# gk7205v200 OTG device profile used to, and cameras in the field still have it
# there; drop the second branch once those have been updated.
USB_MODE=/usr/sbin/usb-mode
[ -x "$USB_MODE" ] || USB_MODE=/usr/bin/usb-mode

if [ ! -x "$USB_MODE" ]; then
	printf '{"ok":false,"supported":false}'
	exit 0
fi

esc() { sed 's/\\/\\\\/g; s/"/\\"/g' | tr -d '\n'; }

ROLE=""
for param in $(echo "$QUERY_STRING" | tr '&' ' '); do
	case "$param" in
		role=*) ROLE="${param#*=}" ;;
	esac
done

# Which /dev/videoN the port ended up on.
#
# Reported rather than assumed, because it is not a fixed answer. The kernel
# does not reuse a v4l2 minor freed earlier in the same boot: take away the
# host-mode webcam that owned /dev/video0 and the gadget composed a moment
# later still comes up as /dev/video1, with video0 sitting free. majestic is
# told a path, so a stale one means it opens nothing and says "gadget not
# started" -- a switch that reports success and leaves a camera the PC cannot
# see. Measured on gk7205v200; it survives waiting for the bus to empty first,
# so it is the kernel's allocator and not a race.
#
# The gadget's node is the one whose sysfs parent is the UDC. Anything else is
# a capture device, which in host mode is the webcam.
video_node() {
	for _v in /sys/class/video4linux/video*; do
		[ -e "$_v" ] || continue
		case "$(readlink "$_v" 2>/dev/null)" in
			*/gadget/*) _kind=gadget ;;
			*) _kind=capture ;;
		esac
		if [ "$_kind" = "$1" ]; then
			echo "/dev/${_v##*/}"
			return 0
		fi
	done
	return 1
}

# Anything a person can see the camera doing, so the page can say it.
report() {
	_st=$("$USB_MODE" status 2>/dev/null)
	_role=$(echo "$_st" | sed -n 's/^dwc3 role  *: *//p')
	_saved=$(echo "$_st" | sed -n 's/^persisted  *: *//p')
	_gadget=false
	echo "$_st" | grep -q '^gadget *: composed' && _gadget=true

	# Root hubs are the controller talking about itself; they are on the bus
	# whether or not anything is plugged in, so they are not "attached".
	_devs=$(lsusb 2>/dev/null | grep -v ' 1d6b:' |
		sed 's/^Bus [0-9]* Device [0-9]*: ID //' | esc)

	if [ "$_role" = device ]; then
		_node=$(video_node gadget)
	else
		_node=$(video_node capture)
	fi

	printf '{"ok":true,"supported":true,"role":"%s","persisted":"%s","gadget":%s,"attached":"%s","video":"%s"%s}' \
		"$(echo "$_role" | esc)" "$(echo "$_saved" | esc)" "$_gadget" "$_devs" \
		"$(echo "$_node" | esc)" "$1"
}

if [ -z "$ROLE" ]; then
	report ""
	exit 0
fi

case "$ROLE" in
	host|device) ;;
	*) printf '{"ok":false,"error":"unknown role"}'; exit 0 ;;
esac

# Whether anything still has a video node open. Polled rather than slept
# against: how long majestic takes to let go is not a number worth guessing,
# and guessing high costs the switch a second it does not need.
video_held() {
	for _p in /proc/[0-9]*; do
		for _f in "$_p"/fd/*; do
			[ -L "$_f" ] || continue
			case "$(readlink "$_f" 2>/dev/null)" in
				/dev/video*) return 0 ;;
			esac
		done
	done
	return 1
}

nap() { usleep 100000 2>/dev/null || sleep 1; }

echo "$ROLE" > /etc/usbmode

if pidof majestic >/dev/null 2>&1; then
	killall -QUIT majestic
	i=0
	while [ $i -lt 50 ] && video_held; do
		i=$((i + 1))
		nap
	done
fi

# The caller follows this with a config write, and that write is what rebuilds
# the pipeline. If it never arrives -- the tab was closed, the network dropped,
# the browser went to sleep mid-switch -- the camera would sit with no video at
# all and no way to notice. So arm a reload that fires regardless. It is free
# when the config write beat it: majestic compares the file against what it
# already applied and leaves an unchanged pipeline alone.
(sleep 12; pidof majestic >/dev/null 2>&1 && killall -HUP majestic) >/dev/null 2>&1 &

if out=$("$USB_MODE" "$ROLE" 2>&1); then
	report ''
else
	# The role did not take. Video still comes back -- the reload above sees to
	# that -- but the port is in a state only a reboot is known to clear, so
	# raise the banner every page already knows how to show.
	touch /tmp/system-reboot
	report ",\"error\":\"$(echo "$out" | esc)\""
fi
