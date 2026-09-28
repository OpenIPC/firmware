#!/bin/sh
#
# Wi-Fi onboarding from a QR code held up to the lens.
#
# The camera reads the code from its own snapshots, tries the network it
# names, and keeps the credentials only once it is actually on that network.
# Each step is announced through the IR-cut filter (majestic's /night/chime),
# because the person holding the phone has no other way to know what the
# camera made of it:
#
#   scanned    two quick rising clicks   the code was read
#   connected  a rising four-note run    joined and got an address; rebooting
#   wrongkey   three low buzzes          the network refused the password
#   nonetwork  down-down, twice          no network by that name answered
#   nodhcp     a long tone, then a low   joined, but nothing handed out an address
#   badqr      one low blip              a code was read, but not a Wi-Fi one
#   timeout    three falling notes       no code seen; scanning stopped
#
# After a failed attempt it reconnects to the network already saved, if there
# is one, and keeps scanning, so a corrected code can be shown straight away.
# The same code is tried again only after RETRY seconds, and does not extend
# the scan: WINDOW counts from the last new code.
#
# Two payloads are understood: the two lines OpenIPC's generator writes
# (wlanssid=... / wlanpass=...), and the WIFI: URI a phone's "share network"
# screen shows.

IFACE=wlan0
STATE=${STATE:-}
WINDOW=${QRSCAN_WINDOW:-30}	# seconds without a code before giving up
RETRY=${QRSCAN_RETRY:-15}	# seconds before the same code is tried again
JOIN_S=${QRSCAN_JOIN_S:-25}	# seconds to wait for the network to accept us

say() {
	logger -t qrscan "$*"
}

# The filter is majestic's to drive; loopback needs no credential, so this
# works on a camera nobody has claimed yet. Silent on firmware without it.
chime() {
	timeout 3 wget -q -O /dev/null "http://127.0.0.1/night/chime?cue=$1" 2>/dev/null
}

# Print the SSID on the first line and the password on the second, or fail for
# a payload that is not a Wi-Fi one. Two lines rather than two words because
# either may contain spaces -- the old script split on them -- and a newline
# is the one character a QR code's Wi-Fi fields cannot carry.
qr_wifi_parse() {
	case "$1" in
	WIFI:*)
		printf '%s' "$1" | awk '
		function field(t) {
			if (substr(t, 1, 2) == "S:") ssid = substr(t, 3)
			else if (substr(t, 1, 2) == "P:") pass = substr(t, 3)
		}
		BEGIN { RS = "\001" }
		{
			s = substr($0, 6); tok = ""; esc = 0
			for (i = 1; i <= length(s); i++) {
				c = substr(s, i, 1)
				if (esc) { tok = tok c; esc = 0 }
				else if (c == "\\") esc = 1
				else if (c == ";") { field(tok); tok = "" }
				else if (c != "\r" && c != "\n") tok = tok c
			}
			field(tok)
		}
		END {
			if (ssid == "") exit 1
			printf "%s\n%s\n", ssid, pass
		}'
		;;
	*wlanssid=*)
		ssid=$(printf '%s\n' "$1" | tr -d '\r' | sed -n 's/^wlanssid=//p' | head -n 1)
		pass=$(printf '%s\n' "$1" | tr -d '\r' | sed -n 's/^wlanpass=//p' | head -n 1)
		[ -n "$ssid" ] || return 1
		printf '%s\n%s\n' "$ssid" "$pass"
		;;
	*)
		return 1
		;;
	esac
}

# What wpa_supplicant's own output says after $2 seconds: wrongkey,
# nonetwork, associated, or nothing while it is still trying.
#
# CTRL-EVENT-NETWORK-NOT-FOUND is not in that output -- it goes to control
# sockets only -- but an attempt is: "Trying to associate" appears within a
# few seconds whenever a network by that name answers. None by NOSUCH_S means
# none is there.
NOSUCH_S=${QRSCAN_NOSUCH_S:-10}
qr_wifi_classify() {
	if grep -q -e 'reason=WRONG_KEY' -e '4-Way Handshake failed' \
		-e 'pre-shared key may be incorrect' "$1"; then
		echo wrongkey
	elif grep -q 'CTRL-EVENT-CONNECTED' "$1"; then
		echo associated
	elif [ "$2" -ge "$NOSUCH_S" ] && ! grep -q 'Trying to a' "$1"; then
		echo nonetwork
	fi
}

# Try the network. Prints connected, wrongkey, nonetwork or nodhcp; leaves
# the interface up and addressed only for connected.
qr_wifi_try() {
	# A WPA passphrase is 8 to 63 characters; nothing else can be right.
	if [ -n "$2" ] && [ ${#2} -lt 8 ]; then
		echo wrongkey
		return
	fi
	# The same configuration the boot path will build from the saved
	# settings, so what joins now joins after the reboot.
	if ! wlan_conf "$1" "$2" > "$STATE/wpa.conf"; then
		echo wrongkey
		return
	fi

	ifdown -f "$IFACE" >/dev/null 2>&1
	killall -q wpa_supplicant
	ip link set "$IFACE" up 2>/dev/null
	wpa_supplicant -i "$IFACE" -D nl80211,wext -c "$STATE/wpa.conf" \
		> "$STATE/wpa.log" 2>&1 &
	pid=$!

	outcome=
	t=0
	while [ $t -lt "$JOIN_S" ]; do
		sleep 1
		t=$((t + 1))
		outcome=$(qr_wifi_classify "$STATE/wpa.log" $t)
		[ -n "$outcome" ] && break
	done
	[ -n "$outcome" ] || outcome=nonetwork

	if [ "$outcome" = associated ]; then
		if udhcpc -i "$IFACE" -n -q -t 5 -T 2 >/dev/null 2>&1; then
			echo connected
			return
		fi
		outcome=nodhcp
	fi
	kill "$pid" 2>/dev/null
	ip link set "$IFACE" down 2>/dev/null
	echo "$outcome"
}

# Back onto the network already saved, if there is one: a camera that was
# working before someone showed it a bad code must not stay off the air.
qr_wifi_restore() {
	[ -n "$(fw_printenv -n wlanssid 2>/dev/null)" ] || return 0
	ifup -f "$IFACE" >/dev/null 2>&1
}

qr_main() {
	if ! command -v wpa_supplicant >/dev/null; then
		say "This image has no wpa_supplicant; QR Wi-Fi onboarding needs it"
		exit 1
	fi
	STATE=$(mktemp -d /tmp/qrscan.XXXXXX) || exit 1
	n=0
	last=
	last_at=0

	while [ $n -lt "$WINDOW" ]; do
		timeout 2 wget -q -O "$STATE/image.jpg" http://127.0.0.1/image.jpg
		data=$(qrscan -p "$STATE/image.jpg" 2>/dev/null | grep -v '^  ERROR')
		rm -f "$STATE/image.jpg"
		now=$(cut -d. -f1 /proc/uptime)

		if [ -n "$data" ] && { [ "$data" != "$last" ] || [ $((now - last_at)) -ge "$RETRY" ]; }; then
			# Only a new code restarts the window; one left in view is
			# retried, but cannot keep the scanner running for ever.
			[ "$data" != "$last" ] && n=0
			last=$data
			if creds=$(qr_wifi_parse "$data"); then
				ssid=$(printf '%s\n' "$creds" | sed -n 1p)
				pass=$(printf '%s\n' "$creds" | sed -n 2p)
				chime scanned
				say "Code read, trying network \"$ssid\""
				outcome=$(qr_wifi_try "$ssid" "$pass")
				chime "$outcome"
				if [ "$outcome" = connected ]; then
					fw_setenv wlanssid "$ssid"
					fw_setenv wlanpass "$pass"
					say "Joined \"$ssid\"; credentials saved, rebooting"
					curl -s --data-binary @/usr/share/openipc/sounds/ready_48k.pcm \
						http://127.0.0.1/play_audio >/dev/null 2>&1
					sleep 3
					reboot
					exit 0
				fi
				say "Network \"$ssid\" not joined: $outcome"
				qr_wifi_restore
			else
				chime badqr
				say "Code read, but it is not a Wi-Fi code"
			fi
			last_at=$(cut -d. -f1 /proc/uptime)
		fi
		sleep 1
		n=$((n + 1))
	done

	chime timeout
	say "No usable code in ${WINDOW} attempts; scanning stopped"
	rm -rf "$STATE"
	exit 1
}

[ -n "$QRSCAN_LIB" ] || qr_main
