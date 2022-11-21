################################################################################
#
# ingenic-osdrv-t20
#
################################################################################

INGENIC_OSDRV_T20_VERSION =
INGENIC_OSDRV_T20_SITE =
INGENIC_OSDRV_T20_LICENSE = MIT
INGENIC_OSDRV_T20_LICENSE_FILES = LICENSE

define INGENIC_OSDRV_T20_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/script/S95ingenic

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/sensor/params/*.bin

	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/sensor/params/jxf22.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/sensor/params/jxh42.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/sensor/params/jxh62.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/sensor/params/sc2135.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/sensor/params/sc2232.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.14/ingenic
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/*.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/audio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/audio2.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/sinfo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/tx-isp-t10.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/tx-isp-t20.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/sensor_jxf22.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/sensor_jxh42.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/sensor_jxh62.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/sensor_sc2135.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/kmod/sensor_sc2232.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/script/load*

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors/params
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors/params $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/sensor/params/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t20/files/lib/*.so
endef

$(eval $(generic-package))
