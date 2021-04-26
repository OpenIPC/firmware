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
	# $(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/*

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/sigmastar-osdrv-ssc335/include
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/sigmastar-osdrv-ssc335/include $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/include/*

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/sigmastar-osdrv-ssc335/kmod
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/sigmastar-osdrv-ssc335/kmod $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/*.ko

	# $(INSTALL) -m 755 -d $(BUILD_DIR)/sigmastar-osdrv-ssc335/lib
	# $(INSTALL) -m 644 -t $(BUILD_DIR)/sigmastar-osdrv-ssc335/lib $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/lib/*.so
endef


define SIGMASTAR_OSDRV_SSC335_INSTALL_TARGET_CMDS
	# $(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/sensor/config/*.ini

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.x/sigmastar
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.x/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/*.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.x/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/kmod/usb/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/script/*

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/fonts
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/fonts $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/fonts/*.bin

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/sensor/*.so

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-ssc335/files/lib/*.so
endef

$(eval $(generic-package))






