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

	# The backported cfg80211 is only needed by the xm711/lynx driver. Install
	# it under its own name instead of over the in-tree cfg80211 at the
	# canonical module path: atbm60xx builds against the in-tree module, and so
	# does the kernel-built mac80211. `wifi xm711` loads the renamed copy.
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.103+/kernel/net/wireless
	$(INSTALL) -m 644 $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/rewrite/cfg80211.ko $(TARGET_DIR)/lib/modules/3.10.103+/kernel/net/wireless/cfg80211_xm711.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/script/wifi

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/lib/*.so
endef

# The vendor kernel ships a modules.dep that only lists its own modules, so
# the xiongmai/ modules installed above (dwc_otg, wifi_pdn) and the atbm60xx
# module are invisible to modprobe. LINUX_RUN_DEPMOD is already registered in
# TARGET_FINALIZE_HOOKS (the linux package is processed before this one) and
# has run by the time we get here; a second pass over the merged target makes
# modprobe resolve the driver stack the wireless/usb cases rely on.
define XIONGMAI_OSDRV_XM530_FINALIZE_MODULES
	$(LINUX_RUN_DEPMOD)
endef
XIONGMAI_OSDRV_XM530_TARGET_FINALIZE_HOOKS += XIONGMAI_OSDRV_XM530_FINALIZE_MODULES

$(eval $(generic-package))
