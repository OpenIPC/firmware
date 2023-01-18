################################################################################
#
# allwinner-osdrv-v83x
#
################################################################################

ALLWINNER_OSDRV_V83X_VERSION =
ALLWINNER_OSDRV_V83X_SITE =
ALLWINNER_OSDRV_V83X_LICENSE = MIT
ALLWINNER_OSDRV_V83X_LICENSE_FILES = LICENSE

define ALLWINNER_OSDRV_V83X_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_ALLWINNER_PATH)/package/allwinner-osdrv-v83x/files/script/S95allwinner

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_ALLWINNER_PATH)/package/allwinner-osdrv-v83x/files/sensor/config/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.118/allwinner
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.118/allwinner $(BR2_EXTERNAL_ALLWINNER_PATH)/package/allwinner-osdrv-v83x/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.118/allwinner $(BR2_EXTERNAL_ALLWINNER_PATH)/package/allwinner-osdrv-v83x/files/kmod/gc2053_mipi.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_ALLWINNER_PATH)/package/allwinner-osdrv-v83x/files/script/load*
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_ALLWINNER_PATH)/package/allwinner-osdrv-v83x/files/sample/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_ALLWINNER_PATH)/package/allwinner-osdrv-v83x/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_ALLWINNER_PATH)/package/allwinner-osdrv-v83x/files/lib/*.so
endef

$(eval $(generic-package))
