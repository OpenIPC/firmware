################################################################################
#
# sigmastar-osdrv-ssc335
#
################################################################################

SIGMASTAR_OSDRV_SSC335_VERSION =
SIGMASTAR_OSDRV_SSC335_SITE =
SIGMASTAR_OSDRV_SSC335_LICENSE = MIT
SIGMASTAR_OSDRV_SSC335_LICENSE_FILES = LICENSE
SIGMASTAR_OSDRV_SSC335_INSTALL_STAGING = YES


define SIGMASTAR_OSDRV_SSC335_INSTALL_STAGING_CMDS
	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/sigmastar
	$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/*.h

	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/sigmastar/drivers/sensorif
	$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/sigmastar/drivers/sensorif $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/drivers/sensorif/*.h

	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/sigmastar/isp/ispahan
	$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/sigmastar/isp/ispahan $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/isp/ispahan/*.h

	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/sigmastar/isp/macaron
	$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/sigmastar/isp/macaron $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/isp/macaron/*.h

	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/sigmastar/isp/pudding
	$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/sigmastar/isp/pudding $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/isp/pudding/*.h


	# $(INSTALL) -m 755 -d $(BUILD_DIR)/usr/include/sigmastar
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/usr/include/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/*.h

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/usr/include/sigmastar/drivers/sensorif
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/usr/include/sigmastar/drivers/sensorif $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/drivers/sensorif/*.h

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/usr/include/sigmastar/isp/ispahan
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/usr/include/sigmastar/isp/ispahan $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/isp/ispahan/*.h

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/usr/include/sigmastar/isp/macaron
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/usr/include/sigmastar/isp/macaron $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/isp/macaron/*.h

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/usr/include/sigmastar/isp/pudding
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/usr/include/sigmastar/isp/pudding $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/isp/pudding/*.h

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/sigmastar-osdrv-ssc335/kmod
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/sigmastar-osdrv-ssc335/kmod $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/*.ko

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/sigmastar-osdrv-ssc335/lib
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/sigmastar-osdrv-ssc335/lib $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/lib/*.so
endef


define SIGMASTAR_OSDRV_SSC335_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/script/S99sigmastar

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iqfile
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/sensor/iqfile/imx307_iqfile.bin
	ln -sf imx307_iqfile.bin $(TARGET_DIR)/etc/sensors/iqfile/iqfile0.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/venc_fw
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/venc_fw $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/sensor/venc_fw/chagall.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mhal.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_common.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_rgn.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_divp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_ipu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_vpe.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_sensor.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_vif.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/mi_shadow.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/sensor/sensor_*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/script/debug_level

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/fonts
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/fonts $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/fonts/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/lib/*.so
endef

$(eval $(generic-package))






