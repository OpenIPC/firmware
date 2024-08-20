################################################################################
#
# wifibroadcast
#
################################################################################

WIFIBROADCAST_VERSION = 24.08
WIFIBROADCAST_SITE = $(call github,svpcom,wfb-ng,wfb-ng-$(WIFIBROADCAST_VERSION))
WIFIBROADCAST_LICENSE = GPL-2.0

WIFIBROADCAST_UNIT = drone
ifeq ($(OPENIPC_SOC_FAMILY),hi3536dv100)
	WIFIBROADCAST_UNIT = gs
endif

WIFIBROADCAST_DEPENDENCIES += libpcap libsodium iw

WIFIBROADCAST_MAKE_OPTS = \
	CC="$(TARGET_CC)" \
	CXX="$(TARGET_CXX)"

define WIFIBROADCAST_BUILD_CMDS
	$(MAKE) $(WIFIBROADCAST_MAKE_OPTS) -C $(@D) all_bin
endef

define WIFIBROADCAST_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/wfb_tx $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 -D $(@D)/wfb_tx_cmd $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 -D $(@D)/wfb_rx $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 -D $(@D)/wfb_keygen $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp $(WIFIBROADCAST_PKGDIR)/files/wfb_$(WIFIBROADCAST_UNIT).conf $(TARGET_DIR)/etc/wfb.conf
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(WIFIBROADCAST_PKGDIR)/files/wifibroadcast $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp $(WIFIBROADCAST_PKGDIR)/files/$(WIFIBROADCAST_UNIT).key $(TARGET_DIR)/etc/$(WIFIBROADCAST_UNIT).key

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(WIFIBROADCAST_PKGDIR)/files/setmcs $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware/ath9k_htc
	cp $(WIFIBROADCAST_PKGDIR)/files/htc_9271.fw.1 $(TARGET_DIR)/lib/firmware/ath9k_htc
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware/ath9k_htc
	cp $(WIFIBROADCAST_PKGDIR)/files/htc_9271.fw.3 $(TARGET_DIR)/lib/firmware/ath9k_htc
endef

$(eval $(generic-package))
