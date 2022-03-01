################################################################################
#
# anyka-osdrv-ak3918ev200
#
################################################################################

ANYKA_OSDRV_AK3918EV200_VERSION =
ANYKA_OSDRV_AK3918EV200_SITE =
ANYKA_OSDRV_AK3918EV200_LICENSE = MIT
ANYKA_OSDRV_AK3918EV200_LICENSE_FILES = LICENSE

define ANYKA_OSDRV_AK3918EV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_ANYKA_PATH)/package/anyka-osdrv-ak3918ev200/files/script/S95anyka

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_ANYKA_PATH)/package/anyka-osdrv-ak3918ev200/files/sensor/config/*.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.4.35/anyka
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.4.35/anyka $(BR2_EXTERNAL_ANYKA_PATH)/package/anyka-osdrv-ak3918ev200/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_ANYKA_PATH)/package/anyka-osdrv-ak3918ev200/files/script/load_anyka
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_ANYKA_PATH)/package/anyka-osdrv-ak3918ev200/files/script/ircut_demo

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_ANYKA_PATH)/package/anyka-osdrv-ak3918ev200/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib

	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_ANYKA_PATH)/package/anyka-osdrv-ak3918ev200/files/lib/xxx.so
endef

$(eval $(generic-package))
