#!/bin/sh
#
# Perform basic settings on a known IP camera
#
#
# Set sensor
#
fw_setenv sensor sc2235
#
# Set custom upgrade url
#
fw_setenv upgrade 'https://github.com/OpenIPC/builder/releases/download/latest/hi3516ev200_lite_imou-cue2-c22en-nor.tgz'
#
# Set reset button GPIO
#
fw_setenv gpio_button 56
#
# Set custom majestic settings
#
cli -s .isp.sensorConfig /etc/sensors/sc2235_i2c_dc_1080p.ini
cli -s .image.flip true
cli -s .image.mirror true
cli -s .audio.enabled true
cli -s .audio.volume 30
cli -s .audio.srate 8000
cli -s .audio.codec opus
cli -s .audio.outputEnabled true
cli -s .audio.outputVolume 100
cli -s .audio.speakerPin 53
cli -s .nightMode.irCutPin1 55
cli -s .nightMode.overrideDrc 150
cli -s .isp.slowShutter low
cli -s .osd.enabled true
cli -s .osd.font /usr/share/fonts/truetype/UbuntuMono-Regular.ttf
cli -s .osd.template "%d.%m.%Y %H:%M:%S"
cli -s .osd.posX 16
cli -s .osd.posY 16
cli -s .rtsp.enabled true
cli -s .rtsp.port 554
cli -s .rtsp.backchannel true

exit 0
