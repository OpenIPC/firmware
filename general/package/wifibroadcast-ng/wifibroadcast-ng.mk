################################################################################
#
# wifibroadcast-ng
#
################################################################################

WIFIBROADCAST_NG_VERSION = 3a053040442174e6c1ce76866c6da4b12c19dbb4
WIFIBROADCAST_NG_SITE = $(call github,svpcom,wfb-ng,$(WIFIBROADCAST_NG_VERSION))
WIFIBROADCAST_NG_LICENSE = GPL-3.0

WIFIBROADCAST_NG_DEPENDENCIES += libpcap libsodium iw

define WIFIBROADCAST_NG_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) LDFLAGS=-s -C $(@D) all_bin
endef

define WIFIBROADCAST_NG_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_NG_PKGDIR)/files/drone.key
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_NG_PKGDIR)/files/wfb.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_rx
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_tx
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_tun
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_NG_PKGDIR)/files/wifibroadcast
endef

$(eval $(generic-package))
