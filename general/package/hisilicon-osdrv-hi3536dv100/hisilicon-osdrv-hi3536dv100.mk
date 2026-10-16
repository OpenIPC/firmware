################################################################################
#
# hisilicon-osdrv-hi3536dv100
#
################################################################################

HISILICON_OSDRV_HI3536DV100_VERSION =
HISILICON_OSDRV_HI3536DV100_SITE =
HISILICON_OSDRV_HI3536DV100_LICENSE = MIT
HISILICON_OSDRV_HI3536DV100_LICENSE_FILES = LICENSE

# The kernel modules are built by hisilicon-opensdk: OSAL and the drivers from
# the SDK's source, the closed MPP modules relinked from its .ko files. The
# libraries here are from the same SDK release (Hi3536DV100_SDK_V2.0.2.0), so
# libmpi and the modules agree on every ioctl (#1621).
define HISILICON_OSDRV_HI3536DV100_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3536DV100_PKGDIR)/files/lib/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3536DV100_PKGDIR)/files/script/*
endef

$(eval $(generic-package))
