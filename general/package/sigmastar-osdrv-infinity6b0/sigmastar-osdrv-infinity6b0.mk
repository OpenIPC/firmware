################################################################################
#
# sigmastar-osdrv-infinity6b0
#
################################################################################

SIGMASTAR_OSDRV_INFINITY6B0_VERSION =
SIGMASTAR_OSDRV_INFINITY6B0_SITE =
SIGMASTAR_OSDRV_INFINITY6B0_LICENSE = MIT
SIGMASTAR_OSDRV_INFINITY6B0_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_INFINITY6B0_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/firmware $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/sensors/firmware/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/sensors/configs/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/script/*
endef

$(eval $(generic-package))
