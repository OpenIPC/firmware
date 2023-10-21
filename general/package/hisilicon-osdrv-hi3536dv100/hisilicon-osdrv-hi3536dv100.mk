################################################################################
#
# hisilicon-osdrv-hi3536dv100
#
################################################################################

HISILICON_OSDRV_HI3536DV100_VERSION =
HISILICON_OSDRV_HI3536DV100_SITE =
HISILICON_OSDRV_HI3536DV100_LICENSE = MIT
HISILICON_OSDRV_HI3536DV100_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3536DV100_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3536DV100_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3536DV100_PKGDIR)/files/lib/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3536DV100_PKGDIR)/files/script/*
endef

$(eval $(generic-package))
