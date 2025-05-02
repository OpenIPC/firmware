################################################################################
#
# rockchip-osdrv-rv11xx
#
################################################################################

ROCKCHIP_OSDRV_RV11XX_VERSION =
ROCKCHIP_OSDRV_RV11XX_SITE =
ROCKCHIP_OSDRV_RV11XX_LICENSE = MIT
ROCKCHIP_OSDRV_RV11XX_LICENSE_FILES = LICENSE

define ROCKCHIP_OSDRV_RV11XX_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/sensors $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/sensors/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/5.10.160/rockchip
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.160/rockchip $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/script/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/lib/*
endef

$(eval $(generic-package))
