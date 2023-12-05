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

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/60fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/60fps $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/config/60fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/iq/imx307.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/iq/imx335.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/iq/sc2232.ini
	ln -sf imx307.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/cipher_drv.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gfbg.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_acodec.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_adc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_chnl.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_h265e.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_isp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_jpege.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_rgn.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_sys.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_tde.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_vedu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_vi.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_vo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_vpss.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/gk7205v200_wdt.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/isp_piris.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/isp_pwm.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/isp_sample_ist.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/isp_sensor_i2c.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/isp_sensor_spi.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/mipi_rx.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/osal.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/ssp_ota5182_ex.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/ssp_st7789_ex.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/ssp_st7796_ex.ko
	### $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/goke $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/kmod/sysconfig.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/*.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_ar0237.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_f23.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_f37.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_gc2053_forcar.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_gc2053.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_gc4653_2l.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_imx290.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_imx307_2l.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_imx307.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_imx327_2l.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_imx327.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_imx335.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_os05a.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_ov2718.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc200ai.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc2231.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc2232h.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc2235.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc2239.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc223a.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc223a_4l.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc2335.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc3235.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc3335.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc4236.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_sc500ai.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/sensor/libsns_bt656.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/lib/*
endef

$(eval $(generic-package))
