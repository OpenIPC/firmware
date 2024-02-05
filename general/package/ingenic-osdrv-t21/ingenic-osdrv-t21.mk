################################################################################
#
# ingenic-osdrv-t21
#
################################################################################

INGENIC_OSDRV_T21_VERSION =
INGENIC_OSDRV_T21_SITE =
INGENIC_OSDRV_T21_LICENSE = MIT
INGENIC_OSDRV_T21_LICENSE_FILES = LICENSE

ifeq ($(OPENIPC_SNS_MODEL),)
define INGENIC_OSDRV_T21_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/*.yaml

	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/jxf23-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/jxf37-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/ov2735b-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/sc2232-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/sc2300-t21.bin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/sc2335-t21.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T21_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(INGENIC_OSDRV_T21_PKGDIR)/files/lib/*.so
endef
else
define INGENIC_OSDRV_T21_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/$(OPENIPC_SNS_MODEL).yaml
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T21_PKGDIR)/files/sensor/params/$(OPENIPC_SNS_MODEL)-$(OPENIPC_SOC_MODEL).bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T21_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(INGENIC_OSDRV_T21_PKGDIR)/files/lib/*.so
endef
endif

$(eval $(generic-package))
