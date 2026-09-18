#!/bin/sh
#
# Perform basic settings on a known IP camera
#
#
# Set SoC and sensor
#
fw_setenv soc hi3516ev200
fw_setenv sensor sc2235
fw_setenv sensor_dvp 1
#
# Set custom upgrade url
#
fw_setenv upgrade 'https://github.com/OpenIPC/builder/releases/download/latest/hi3516ev200_lite_imou-cue2-c22en-nor.tgz'
#
# Set reset button GPIO
#
fw_setenv gpio_button 56
#
# Set wireless device and MAC address
#
fw_setenv wlandev rtl8188fu-hi3516ev200-imou-cue2

exit 0
