################################################################################
#
# xiongmai-osdrv-xm510
#
################################################################################

XIONGMAI_OSDRV_XM510_VERSION =
XIONGMAI_OSDRV_XM510_SITE =
XIONGMAI_OSDRV_XM510_LICENSE = MIT
XIONGMAI_OSDRV_XM510_LICENSE_FILES = LICENSE

define XIONGMAI_OSDRV_XM510_INSTALL_TARGET_CMDS
	# $(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(XIONGMAI_OSDRV_XM510_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.0.101/xiongmai
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.101/xiongmai $(XIONGMAI_OSDRV_XM510_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM510_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM510_PKGDIR)/files/bin/isp

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(XIONGMAI_OSDRV_XM510_PKGDIR)/files/sensor/*.so

	# $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(XIONGMAI_OSDRV_XM510_PKGDIR)/files/lib/*.so
endef

$(eval $(generic-package))
