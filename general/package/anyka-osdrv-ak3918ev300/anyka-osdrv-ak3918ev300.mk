################################################################################
#
# anyka-osdrv-ak3918ev300
#
################################################################################

ANYKA_OSDRV_AK3918EV300_VERSION =
ANYKA_OSDRV_AK3918EV300_SITE =
ANYKA_OSDRV_AK3918EV300_LICENSE = PROPRIETARY

# files/sensor/<sensor>.conf is the ISP tuning of one sensor, named as
# ipctool names the sensor (`ipcinfo -s`). The kernel's ISP driver
# (drivers/misc/ak_isp_tuning.c) reads /etc/sensor/<name>.conf for the
# sensor it detected when capture first starts, and falls back to
# anyka/isp.conf for a sensor with no file here. On this ISP the sensor's
# init register list lives in the tuning, so a sensor cannot start
# without one.
#
# Each file is a vendor tuning from the stock firmware of an AK3918EV300
# camera, unchanged: the kernel validates the EV300 "4.003" layout as it
# is, so no conversion is needed.
#
#   jxf37.conf   isp_f37_mipi_1lane.conf  4.003-mp1l-9.7
#                HeimVision PE3020-W (OpenIPC/builder#234),
#                sha256 2ed7d02c3fe3dba3e96d31312a7b28d858a796ad0254e5fbc110228c2ae6be5c

define ANYKA_OSDRV_AK3918EV300_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor \
		$(ANYKA_OSDRV_AK3918EV300_PKGDIR)/files/sensor/*.conf
endef

$(eval $(generic-package))
