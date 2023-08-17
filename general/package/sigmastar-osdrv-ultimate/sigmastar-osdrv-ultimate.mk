################################################################################
#
# sigmastar-osdrv-ultimate
#
################################################################################

SIGMASTAR_OSDRV_ULTIMATE_LICENSE = MIT
SIGMASTAR_OSDRV_ULTIMATE_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_ULTIMATE_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(SIGMASTAR_OSDRV_ULTIMATE_PKGDIR)/files/$(OPENIPC_SOC_FAMILY)/lib/*
endef

$(eval $(generic-package))
