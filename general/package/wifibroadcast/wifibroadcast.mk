################################################################################
#
# wifibroadcast
#
################################################################################
WIFIBROADCAST_VERSION = 23.01
WIFIBROADCAST_SITE = $(call github,svpcom,wfb-ng,wfb-ng-$(WIFIBROADCAST_VERSION))
WIFIBROADCAST_LICENSE = GPL-2.0

WIFIBROADCAST_DEPENDENCIES += libpcap libsodium iw

WIFIBROADCAST_MAKE_OPTS = \
	CC="$(TARGET_CC)" \
	CXX="$(TARGET_CXX)"

define WIFIBROADCAST_BUILD_CMDS
	$(MAKE) $(WIFIBROADCAST_MAKE_OPTS) -C $(@D) all
endef

define WIFIBROADCAST_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/wfb_tx $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 -D $(@D)/wfb_rx $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 -D $(@D)/wfb_keygen $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp ../general/package/wifibroadcast/files/wfb.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp ../general/package/wifibroadcast/files/wifibroadcast $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp ../general/package/wifibroadcast/files/setmcs $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware/ath9k_htc
	cp ../general/package/wifibroadcast/files/htc_9271.fw.1 $(TARGET_DIR)/lib/firmware/ath9k_htc
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware/ath9k_htc
	cp ../general/package/wifibroadcast/files/htc_9271.fw.3 $(TARGET_DIR)/lib/firmware/ath9k_htc
endef

$(eval $(generic-package))
