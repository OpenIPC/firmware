################################################################################
#
# sigmastar-osdrv-infinity6e
#
################################################################################

SIGMASTAR_OSDRV_INFINITY6E_VERSION =
SIGMASTAR_OSDRV_INFINITY6E_SITE =
SIGMASTAR_OSDRV_INFINITY6E_LICENSE = MIT
SIGMASTAR_OSDRV_INFINITY6E_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_INFINITY6E_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)/files/sensor/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iqfile
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)/files/sensor/iqfile/iqfile0.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/venc_fw
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/venc_fw $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)/files/sensor/venc_fw/chagall.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)/files/bin/*
endef

$(eval $(generic-package))
