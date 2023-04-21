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
#	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
#	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/script/S95ingenic

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/script/ircut_demo

#	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
#	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-osdrv-t40/files/lib/*.so
endef

$(eval $(generic-package))
