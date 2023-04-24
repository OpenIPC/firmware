################################################################################
#
# ingenic-osdrv-t40
#
################################################################################

INGENIC_OSDRV_T40_VERSION =
INGENIC_OSDRV_T40_SITE =
INGENIC_OSDRV_T40_LICENSE = MIT
INGENIC_OSDRV_T40_LICENSE_FILES = LICENSE

define INGENIC_OSDRV_T40_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/script/S95ingenic

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/*.yaml

	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/gc4653-t40.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/imx307-t40.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/imx415-t40.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/jxk04-t40.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/sc3335-t40.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/sc500ai-t40.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/sc5235-t40.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/sc8238-t40.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/sensor/params/sc830ai-t40.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.4.94/ingenic
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/audio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/avpu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/dtrng_dev.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/gpio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/mpsys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sinfo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/soc-nna.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/tx-isp-t40.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_gc4653_t40.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_imx307_t40.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_imx335_t40.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_imx415_t40.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_jxk04_t40.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_sc2232h_t40.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_sc500ai_t40.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_sc3335_t40.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.4.94/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/kmod/sensor_sc5235_t40.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/lib/*.so
endef

$(eval $(generic-package))
