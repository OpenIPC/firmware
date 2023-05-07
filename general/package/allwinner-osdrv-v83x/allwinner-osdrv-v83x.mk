################################################################################
#
# allwinner-osdrv-v83x
#
################################################################################

ALLWINNER_OSDRV_V83X_VERSION =
ALLWINNER_OSDRV_V83X_SITE =
ALLWINNER_OSDRV_V83X_LICENSE = MIT
ALLWINNER_OSDRV_V83X_LICENSE_FILES = LICENSE

define ALLWINNER_OSDRV_V83X_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(ALLWINNER_OSDRV_V83X_PKGDIR)/files/sensor/config/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.118/allwinner
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.118/allwinner $(ALLWINNER_OSDRV_V83X_PKGDIR)/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.118/allwinner $(ALLWINNER_OSDRV_V83X_PKGDIR)/files/kmod/gc2053_mipi.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(ALLWINNER_OSDRV_V83X_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(ALLWINNER_OSDRV_V83X_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(ALLWINNER_OSDRV_V83X_PKGDIR)/files/lib/*.so
endef

$(eval $(generic-package))
