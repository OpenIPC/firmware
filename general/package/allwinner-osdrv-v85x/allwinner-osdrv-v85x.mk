################################################################################
#
# allwinner-osdrv-v85x
#
################################################################################

ALLWINNER_OSDRV_V85X_VERSION =
ALLWINNER_OSDRV_V85X_SITE =
ALLWINNER_OSDRV_V85X_LICENSE = MIT
ALLWINNER_OSDRV_V85X_LICENSE_FILES = LICENSE

define ALLWINNER_OSDRV_V85X_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(ALLWINNER_OSDRV_V85X_PKGDIR)/files/etc/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(ALLWINNER_OSDRV_V85X_PKGDIR)/files/script/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(ALLWINNER_OSDRV_V85X_PKGDIR)/files/lib/*
endef

$(eval $(generic-package))
