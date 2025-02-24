################################################################################
#
# sigmastar-osdrv-msc313e
#
################################################################################

SIGMASTAR_OSDRV_MSC313E_VERSION =
SIGMASTAR_OSDRV_MSC313E_SITE =
SIGMASTAR_OSDRV_MSC313E_LICENSE = MIT
SIGMASTAR_OSDRV_MSC313E_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_MSC313E_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iqfile
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/*.xml
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/f22*.mz
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/imx307*.mz
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/imx323*.mz
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/sc2235*.mz
#	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/*.mz

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.18.30/sigmastar
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.30/sigmastar $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/bin/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/lib/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*JXF22*.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*imx307*.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*imx323*.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*sc2235*.so
#	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*.so

endef

$(eval $(generic-package))
