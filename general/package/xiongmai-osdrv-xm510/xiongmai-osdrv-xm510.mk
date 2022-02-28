################################################################################
#
# xiongmai-osdrv-xm510
#
################################################################################

XIONGMAI_OSDRV_XM510_VERSION =
XIONGMAI_OSDRV_XM510_SITE =
XIONGMAI_OSDRV_XM510_LICENSE = MIT
XIONGMAI_OSDRV_XM510_LICENSE_FILES = LICENSE

define XIONGMAI_OSDRV_XM510_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_XIONGMAI_PATH)/package/xiongmai-osdrv-xm510/files/script/S95xiongmai

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_XIONGMAI_PATH)/package/xiongmai-osdrv-xm510/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.0.101/xiongmai
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.101/xiongmai $(BR2_EXTERNAL_XIONGMAI_PATH)/package/xiongmai-osdrv-xm510/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_XIONGMAI_PATH)/package/xiongmai-osdrv-xm510/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_XIONGMAI_PATH)/package/xiongmai-osdrv-xm510/files/bin/isp

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_XIONGMAI_PATH)/package/xiongmai-osdrv-xm510/files/sensor/*.so

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_XIONGMAI_PATH)/package/xiongmai-osdrv-xm510/files/lib/*.so
endef

$(eval $(generic-package))
