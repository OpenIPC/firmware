################################################################################
#
# sigmastar-osdrv-infinity6b0
#
################################################################################

SIGMASTAR_OSDRV_INFINITY6B0_VERSION =
SIGMASTAR_OSDRV_INFINITY6B0_SITE =
SIGMASTAR_OSDRV_INFINITY6B0_LICENSE = MIT
SIGMASTAR_OSDRV_INFINITY6B0_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_INFINITY6B0_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/sensor/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iqfile
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/sensor/iqfile/iqfile0.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/venc_fw
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/venc_fw $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/sensor/venc_fw/chagall.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)/files/bin/*
endef

$(eval $(generic-package))
