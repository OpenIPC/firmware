################################################################################
#
# xiongmai-osdrv-xm530
#
################################################################################

XIONGMAI_OSDRV_XM530_VERSION =
XIONGMAI_OSDRV_XM530_SITE =
XIONGMAI_OSDRV_XM530_LICENSE = MIT
XIONGMAI_OSDRV_XM530_LICENSE_FILES = LICENSE

define XIONGMAI_OSDRV_XM530_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/firmware/hevcenc.fwb

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/usb/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/xm711/*.ko

	# Dirty hack
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.103+/kernel/net/wireless
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/kernel/net/wireless $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/rewrite/cfg80211.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/script/wifi

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/lib/*.so
endef

$(eval $(generic-package))
