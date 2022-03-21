################################################################################
#
# sigmastar-osdrv-msc313e
#
################################################################################

SIGMASTAR_OSDRV_MSC313E_VERSION =
SIGMASTAR_OSDRV_MSC313E_SITE =
SIGMASTAR_OSDRV_MSC313E_LICENSE = MIT
SIGMASTAR_OSDRV_MSC313E_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_MSC313E_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-msc313e/files/script/S95sigmastar

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iqfile

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/venc_fw
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/venc_fw $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-msc313e/files/sensor/venc_fw/chagall.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.18.30/sigmastar
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.30/sigmastar $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-msc313e/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-msc313e/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-msc313e/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-msc313e/files/script/debug_level

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_SIGMASTAR_PATH)/package/sigmastar-osdrv-msc313e/files/lib/*.so
endef

$(eval $(generic-package))
