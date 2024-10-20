################################################################################
#
# wifibroadcast
#
################################################################################

WIFIBROADCAST_VERSION = 3a053040442174e6c1ce76866c6da4b12c19dbb4
WIFIBROADCAST_SITE = $(call github,svpcom,wfb-ng,$(WIFIBROADCAST_VERSION))
WIFIBROADCAST_LICENSE = GPL-2.0

WIFIBROADCAST_UNIT = drone
ifeq ($(OPENIPC_SOC_FAMILY),hi3536dv100)
	WIFIBROADCAST_UNIT = gs
endif

WIFIBROADCAST_DEPENDENCIES += libpcap libsodium iw

define WIFIBROADCAST_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) LDFLAGS=-s -C $(@D) all_bin
endef

define WIFIBROADCAST_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -D $(WIFIBROADCAST_PKGDIR)/files/$(WIFIBROADCAST_UNIT).key $(TARGET_DIR)/etc/$(WIFIBROADCAST_UNIT).key
	$(INSTALL) -m 644 -D $(WIFIBROADCAST_PKGDIR)/files/wfb_$(WIFIBROADCAST_UNIT).conf $(TARGET_DIR)/etc/wfb.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware/ath9k_htc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware/ath9k_htc $(WIFIBROADCAST_PKGDIR)/files/htc_9271.fw.1
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware/ath9k_htc $(WIFIBROADCAST_PKGDIR)/files/htc_9271.fw.3

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_rx
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_tx
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_tx_cmd
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_keygen
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/wfb_tun
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_PKGDIR)/files/setmcs
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_PKGDIR)/files/wifibroadcast
endef

$(eval $(generic-package))
