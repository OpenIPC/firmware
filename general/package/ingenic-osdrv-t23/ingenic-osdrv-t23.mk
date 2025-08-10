################################################################################
#
# ingenic-osdrv-t23
#
################################################################################

INGENIC_OSDRV_T23_VERSION =
INGENIC_OSDRV_T23_SITE =
INGENIC_OSDRV_T23_LICENSE = MIT
INGENIC_OSDRV_T23_LICENSE_FILES = LICENSE

ifeq ($(OPENIPC_SNS_MODEL),)
define INGENIC_OSDRV_T23_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T23_PKGDIR)/files/sensor/*.yaml
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T23_PKGDIR)/files/sensor/params/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T23_PKGDIR)/files/script/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(INGENIC_OSDRV_T23_PKGDIR)/files/lib/*
endef
else
define INGENIC_OSDRV_T23_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T23_PKGDIR)/files/sensor/$(OPENIPC_SNS_MODEL).yaml
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(INGENIC_OSDRV_T23_PKGDIR)/files/sensor/params/$(OPENIPC_SNS_MODEL)-$(OPENIPC_SOC_MODEL).bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(INGENIC_OSDRV_T23_PKGDIR)/files/script/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(INGENIC_OSDRV_T23_PKGDIR)/files/lib/*
endef
endif

$(eval $(generic-package))
