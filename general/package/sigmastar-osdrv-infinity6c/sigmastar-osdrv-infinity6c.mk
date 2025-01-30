################################################################################
#
# sigmastar-osdrv-infinity6c
#
################################################################################

SIGMASTAR_OSDRV_INFINITY6C_VERSION =
SIGMASTAR_OSDRV_INFINITY6C_SITE =
SIGMASTAR_OSDRV_INFINITY6C_LICENSE = MIT
SIGMASTAR_OSDRV_INFINITY6C_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_INFINITY6C_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/5.10.61/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.61/sigmastar $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/firmware $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)/files/sensor/firmware/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)/files/sensor/configs/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)/files/lib/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)/files/script/*
endef

$(eval $(generic-package))
