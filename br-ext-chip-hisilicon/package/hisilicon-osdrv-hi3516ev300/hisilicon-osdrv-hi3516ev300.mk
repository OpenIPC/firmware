################################################################################
#
# hisilicon-osdrv-hi3516ev300
#
################################################################################

HISILICON_OSDRV_HI3516EV300_VERSION =
HISILICON_OSDRV_HI3516EV300_SITE =
HISILICON_OSDRV_HI3516EV300_LICENSE = MIT
HISILICON_OSDRV_HI3516EV300_LICENSE_FILES = LICENSE
HISILICON_OSDRV_HI3516EV300_INSTALL_STAGING = YES


define HISILICON_OSDRV_HI3516EV300_INSTALL_STAGING_CMDS
	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/hisilicon
	$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/include/*

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/hisilicon-osdrv-hi3516ev300/include
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/hisilicon-osdrv-hi3516ev300/include $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/include/*

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/hisilicon-osdrv-hi3516ev300/kmod
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/hisilicon-osdrv-hi3516ev300/kmod $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/*.ko

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/hisilicon-osdrv-hi3516ev300/lib
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/hisilicon-osdrv-hi3516ev300/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/lib/*.so
endef


define HISILICON_OSDRV_HI3516EV300_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/script/S99hisilicon

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/WDR
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/WDR $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/sensor/config/WDR/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/cma_osal.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_acodec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_chnl.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_h265e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_isp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_jpege.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_rgn.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_tde.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_vedu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_vi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_vpss.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi3516ev200_wdt.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi_mipi_rx.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi_osal.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi_pwm.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi_sensor_i2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/hi_sensor_spi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/kmod/sys_config.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/fonts
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/fonts $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/fonts/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev300/files/lib/*.so
endef

$(eval $(generic-package))
