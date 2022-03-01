#!/bin/sh

wlan=$1
chan="6"

ifconfig ${wlan} down
iwconfig ${wlan} mode monitor
ifconfig ${wlan} up
iwconfig ${wlan} channel ${chan}

wfb_tx -p 1 -u 5600 -K /tmp/drone.key ${wlan}
