################################################################################
#
# anyka-osdrv-ak3918ev200
#
################################################################################

ANYKA_OSDRV_AK3918EV200_VERSION =
ANYKA_OSDRV_AK3918EV200_SITE =
ANYKA_OSDRV_AK3918EV200_LICENSE = PROPRIETARY

# files/sensor/<sensor>.conf is the ISP tuning of one sensor, named as
# ipctool names the sensor (`ipcinfo -s`), not always as the vendor did.
# The kernel's ISP driver (drivers/misc/ak_isp_tuning.c) reads
# /etc/sensor/<name>.conf for the
# sensor it detected when capture first starts, so every file here is
# reachable and only the detected sensor's is read.
#
# Every file is a vendor tuning from the stock firmware of an AK3918EV200
# camera, converted from the vendor's "3.0xx" layout to the "3.05_032401"
# one the kernel validates with files/sensor/convert-3.004.py. The
# conversion changes header fields only (version string, style count and
# style size) and gives each style style 0's sensor register list, the
# only one the kernel writes; no tuning value is changed.
#
#   gc1004.conf  isp_gc1024.conf      v3.000      AK3918EV200 camera
#   gc1034.conf  isp_gc1034_0.conf    v3.001      V380E2 camera
#   jxh62.conf   isp_h62.conf         v3.004      AnyCloud39EV300 SDK
#                (UVC_check_sensor; a webcam tuning, the only H62 one)
#   sc1135.conf  isp_sc1135_0.conf    v3.015      V380E2 camera
#   sc1145.conf  isp_sc1145.conf      v3.006      AK3918EV200 camera
#   sc1235.conf  isp_sc1235.conf      v3.012      AK3918EV200 camera
#   sc1345.conf  isp_sc1345_dvp.conf  v3.004      Gwell camera
#   sc2145h.conf isp_sc1245.conf      v3.011      AK3918EV200 camera
#   sc2235p.conf isp_sc2232.conf      v3.000      V380E2 camera
#
# Where a camera carried several, the newest generic one was taken. The
# EV300 and EV330 tunings ("4.x", "5.x") are laid out for other ISP
# libraries and are not usable here.

define ANYKA_OSDRV_AK3918EV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor \
		$(ANYKA_OSDRV_AK3918EV200_PKGDIR)/files/sensor/*.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin \
		$(ANYKA_OSDRV_AK3918EV200_PKGDIR)/files/script/load_anyka
endef

$(eval $(generic-package))
