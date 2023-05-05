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
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/sensor/config/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.111/rockchip
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.111/rockchip $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(ROCKCHIP_OSDRV_RV11XX_PKGDIR)/files/lib/*.so
endef

$(eval $(generic-package))
