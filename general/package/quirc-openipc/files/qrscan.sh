#!/bin/sh

if [ -e /usr/share/openipc/gpio.conf ]; then
        . /usr/share/openipc/gpio.conf
fi

gpio=${led1}

n=0

gpio toggle ${gpio} | logger -t gpio

while true ; do
    if [ "$n" -ge 30 ]; then
        logger -t qrscan "Recognition timeout exceeded, reboot camera and try again..."
        gpio toggle ${gpio} | logger -t gpio
        exit 1
    fi
    timeout 1 wget -q -O /tmp/image.jpg http://127.0.0.1/image.jpg
    data=$(qrscan -p /tmp/image.jpg)
    if [[ -n "$data" ]] && $(echo "$data" | grep -q wlan); then
        fw_setenv $(echo $data | cut -d " " -f 1 | sed 's/=/ /')
        fw_setenv $(echo $data | cut -d " " -f 2 | sed 's/=/ /')
        logger -t qrscan "Recognition successfully, wlanssid and wlanpass is writed to env. Reboot required."
        curl --data-binary @/usr/share/openipc/sounds/ready_48k.pcm http://localhost/play_audio
	for a in $(seq 10) ; do (gpio toggle ${gpio} ; sleep 0.3 ; gpio toggle ${gpio} ; sleep 0.3) ; done >/dev/null 2>&1
        reboot -f
    fi
    sleep 1
    n=$((n + 1))
done
