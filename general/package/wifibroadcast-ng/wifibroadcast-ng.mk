################################################################################
#
# wifibroadcast-ng
#
################################################################################

WIFIBROADCAST_NG_VERSION = 3648da920699c75ae349a2627b8e27fa680be941
WIFIBROADCAST_NG_SITE = $(call github,svpcom,wfb-ng,$(WIFIBROADCAST_NG_VERSION))
WIFIBROADCAST_NG_LICENSE = GPL-3.0

WIFIBROADCAST_NG_DEPENDENCIES += libpcap libsodium

define WIFIBROADCAST_NG_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) LDFLAGS=-s -C $(@D) all_bin
endef

define WIFIBROADCAST_NG_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_NG_PKGDIR)/files/drone.key
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_NG_PKGDIR)/files/wfb.yaml

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_NG_PKGDIR)/files/S98wifibroadcast

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(WIFIBROADCAST_NG_PKGDIR)/sensor/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_rx
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_tx
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_tx_cmd
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_tun
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_NG_PKGDIR)/files/wifibroadcast
endef

$(eval $(generic-package))
