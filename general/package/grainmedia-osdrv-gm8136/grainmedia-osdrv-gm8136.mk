################################################################################
#
# grainmedia-osdrv-gm8136
#
################################################################################

GRAINMEDIA_OSDRV_GM8136_VERSION =
GRAINMEDIA_OSDRV_GM8136_SITE =
GRAINMEDIA_OSDRV_GM8136_LICENSE = MIT
GRAINMEDIA_OSDRV_GM8136_LICENSE_FILES = LICENSE

define GRAINMEDIA_OSDRV_GM8136_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_GRAINMEDIA_PATH)/package/grainmedia-osdrv-gm8136/files/script/S95grainmedia

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_GRAINMEDIA_PATH)/package/grainmedia-osdrv-gm8136/files/sensor/config/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.3.0/grainmedia
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.3.0/grainmedia $(BR2_EXTERNAL_GRAINMEDIA_PATH)/package/grainmedia-osdrv-gm8136/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GRAINMEDIA_PATH)/package/grainmedia-osdrv-gm8136/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GRAINMEDIA_PATH)/package/grainmedia-osdrv-gm8136/files/sample/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_GRAINMEDIA_PATH)/package/grainmedia-osdrv-gm8136/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GRAINMEDIA_PATH)/package/grainmedia-osdrv-gm8136/files/lib/*.so
endef

$(eval $(generic-package))
