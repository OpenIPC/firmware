################################################################################
#
# anyka-osdrv-ak3918ev200
#
################################################################################

ANYKA_OSDRV_AK3918EV200_VERSION =
ANYKA_OSDRV_AK3918EV200_SITE =
ANYKA_OSDRV_AK3918EV200_LICENSE = PROPRIETARY

# files/sensor/<sensor>.conf is the ISP tuning of one sensor, named as
# ipctool names the sensor. The kernel's ISP driver
# (drivers/misc/ak_isp_tuning.c) reads /etc/sensor/<name>.conf for the
# sensor it detected when capture first starts, so every file here is
# reachable and only the detected sensor's is read.
#
# sc1345.conf (SmartSens SC1345, DVP): isp_sc1345_dvp.conf from the stock
# firmware of a Gwell AK3918EV200 camera, converted from the vendor's
# "3.004" layout to the "3.05_032401" one the kernel validates with
# files/sensor/convert-3.004.py. The conversion changes header fields only
# (version string, style count and style size) and gives each style
# style 0's sensor register list; no tuning value is changed.

define ANYKA_OSDRV_AK3918EV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor \
		$(ANYKA_OSDRV_AK3918EV200_PKGDIR)/files/sensor/*.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin \
		$(ANYKA_OSDRV_AK3918EV200_PKGDIR)/files/script/load_anyka
endef

$(eval $(generic-package))
