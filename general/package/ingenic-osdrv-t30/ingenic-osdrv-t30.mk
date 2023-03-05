################################################################################
#
# ingenic-osdrv-t30
#
################################################################################

INGENIC_OSDRV_T30_VERSION =
INGENIC_OSDRV_T30_SITE =
INGENIC_OSDRV_T30_LICENSE = MIT
INGENIC_OSDRV_T30_LICENSE_FILES = LICENSE

define INGENIC_OSDRV_T30_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/script/S95ingenic

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/sensor/*.yaml
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/sensor/params/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.14__isvp_monkey_1.0__/ingenic
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_monkey_1.0__/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/kmod/*.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_monkey_1.0__/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/kmod/audio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_monkey_1.0__/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/kmod/gpio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_monkey_1.0__/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/kmod/sinfo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_monkey_1.0__/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/kmod/tx-isp-t30.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_monkey_1.0__/ingenic $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t30/files/lib/*.so
endef

$(eval $(generic-package))
