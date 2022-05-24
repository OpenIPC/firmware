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
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_ROCKCHIP_PATH)/package/rockchip-osdrv-rv11xx/files/script/S95rockchip

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_ROCKCHIP_PATH)/package/rockchip-osdrv-rv11xx/files/sensor/config/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.111/rockchip
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.111/rockchip $(BR2_EXTERNAL_ROCKCHIP_PATH)/package/rockchip-osdrv-rv11xx/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_ROCKCHIP_PATH)/package/rockchip-osdrv-rv11xx/files/script/load*
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_ROCKCHIP_PATH)/package/rockchip-osdrv-rv11xx/files/sample/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_ROCKCHIP_PATH)/package/rockchip-osdrv-rv11xx/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_ROCKCHIP_PATH)/package/rockchip-osdrv-rv11xx/files/lib/*.so
endef

$(eval $(generic-package))
