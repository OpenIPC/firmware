################################################################################
#
# xiongmai-osdrv-xm530
#
################################################################################

XIONGMAI_OSDRV_XM530_VERSION =
XIONGMAI_OSDRV_XM530_SITE =
XIONGMAI_OSDRV_XM530_LICENSE = MIT
XIONGMAI_OSDRV_XM530_LICENSE_FILES = LICENSE

# xiongmai-opensdk builds open_mmz.ko in place of mmz.ko. Both export the
# same symbols, and depmod would make every module that needs them depend on
# the vendor one, so modprobe open_vi would try to load it over open_mmz.
# So mmz.ko moves out of the tree depmod reads, to where load_xiongmai finds
# it only when open_mmz does not load. (xm530_vi.ko stays: nothing depends on
# its symbols, and open_vi falls back to it by name.)
ifeq ($(BR2_PACKAGE_XIONGMAI_OPENSDK),y)
define XIONGMAI_OSDRV_XM530_DROP_REPLACED
	$(INSTALL) -D -m 644 $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai/mmz.ko \
		$(TARGET_DIR)/usr/lib/xiongmai/mmz.ko
	rm -f $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai/mmz.ko
endef
endif

define XIONGMAI_OSDRV_XM530_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/firmware/hevcenc.fwb

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/*.ko
	$(XIONGMAI_OSDRV_XM530_DROP_REPLACED)
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/usb/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/xm711/*.ko

	# Dirty hack
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.103+/kernel/net/wireless
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/kernel/net/wireless $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/kmod/rewrite/cfg80211.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/script/wifi
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/script/set_allocator

	# Only the vendor ISP library, which has no source. libsns_X50_XM530.so,
	# libsns_X123_XM530.so and libispapp_XM530.so are built from source by
	# xiongmai-opensdk.
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(XIONGMAI_OSDRV_XM530_PKGDIR)/files/lib/libisp_XM530.so
endef

$(eval $(generic-package))
