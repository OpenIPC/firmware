################################################################################
#
# goke-osdrv-gk7205v200
#
################################################################################

GOKE_OSDRV_GK7205V200_VERSION =
GOKE_OSDRV_GK7205V200_SITE =
GOKE_OSDRV_GK7205V200_LICENSE = MIT
GOKE_OSDRV_GK7205V200_LICENSE_FILES = LICENSE
GOKE_OSDRV_GK7205V200_INSTALL_STAGING = YES

define GOKE_OSDRV_GK7205V200_INSTALL_STAGING_CMDS
	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/goke
	#$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/include/*

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/include
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/include $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/include/*

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/kmod
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/kmod $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/*.ko

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/lib
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/lib $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/lib/*.so
endef

define GOKE_OSDRV_GK7205V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/WDR
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/WDR $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/config/WDR/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/high-fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/high-fps $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/config/high-fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/iq/imx307.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/iq/imx335.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/iq/sc2232.ini
	ln -sf imx307.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

	# V4 heavy modules (acodec, adec, aenc, ai, aio, ao, base, chnl, h264e,
	# h265e, ive, jpege, rc, rgn, sys, vedu, venc, vgs, vi, vpss) are
	# built from openhisilicon source and installed by hisilicon-opensdk
	# under /lib/modules/4.9.37/goke/ with the gk7205v200_<mod>.ko names
	# load_goke insmods. Keep the dir creation here so this package is
	# self-sufficient even without opensdk.
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/goke

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# Source-built sensors are installed by hisilicon-opensdk via
	# HISILICON_OPENSDK_SENSORS_gk7205v200. Only binary-only blobs
	# (no source available) are installed here.
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc200ai.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc2232h.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc2239.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc223a_4l.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc2335.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/lib/*
endef

$(eval $(generic-package))
