#!/bin/sh

SN=$(ipcinfo -i)
if [ ! -z "$SN" ]; then
    HOSTNAME="IPC-$SN"
	hostname "$HOSTNAME"
	echo "$HOSTNAME" > /etc/hostname
	echo "127.0.0.1 localhost" > /etc/hosts
	echo "127.0.1.1 ${HOSTNAME}" >> /etc/hosts
fi

wled_pin=$(gpiofind wled_pin | awk '{print $2}')
test -z $wled_pin || yaml-cli -s .nightMode.backlightPin $wled_pin

ircut_pin1=$(gpiofind ircut_pin1 | awk '{print $2}')
ircut_pin2=$(gpiofind ircut_pin2 | awk '{print $2}')
test -z $ircut_pin1 || yaml-cli -s .nightMode.irCutPin1 $ircut_pin1
test -z $ircut_pin2 || yaml-cli -s .nightMode.irCutPin2 $ircut_pin2
