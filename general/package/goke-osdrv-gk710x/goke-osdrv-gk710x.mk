################################################################################
#
# goke-osdrv-gk710x
#
################################################################################

GOKE_OSDRV_GK710X_VERSION =
GOKE_OSDRV_GK710X_SITE =
GOKE_OSDRV_GK710X_LICENSE = MIT
GOKE_OSDRV_GK710X_LICENSE_FILES = LICENSE

ifeq ($(OPENIPC_SOC_MODEL),gk7102)
	GOKE_OSDRV_GK710X_FIRMWARE = gk_fw_710x.bin
else ifeq ($(OPENIPC_SOC_MODEL),gk7102s)
	GOKE_OSDRV_GK710X_FIRMWARE = gk_fw_710xs.bin
endif

define GOKE_OSDRV_GK710X_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(GOKE_OSDRV_GK710X_PKGDIR)/files/sensor/config/*.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(GOKE_OSDRV_GK710X_PKGDIR)/files/sensor/config/sc1135.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(GOKE_OSDRV_GK710X_PKGDIR)/files/sensor/config/gc1034.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware $(GOKE_OSDRV_GK710X_PKGDIR)/files/sensor/fw/$(GOKE_OSDRV_GK710X_FIRMWARE)

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.4.43-Goke/goke
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.4.43-Goke/goke $(GOKE_OSDRV_GK710X_PKGDIR)/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.4.43-Goke/goke $(GOKE_OSDRV_GK710X_PKGDIR)/files/sensor/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK710X_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK710X_PKGDIR)/files/script/ircut_demo

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(GOKE_OSDRV_GK710X_PKGDIR)/files/lib/*.so
endef

$(eval $(generic-package))
