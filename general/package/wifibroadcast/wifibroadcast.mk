################################################################################
#
# wifibroadcast
#
################################################################################

WIFIBROADCAST_VERSION = 51251b8c484b8c4f548aa3bbb1633e0edbb605dc
WIFIBROADCAST_SITE = $(call github,svpcom,wifibroadcast,$(WIFIBROADCAST_VERSION))
WIFIBROADCAST_LICENSE = GPL-2.0

WIFIBROADCAST_DEPENDENCIES += libpcap libsodium iw netcat

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

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp ../general/package/wifibroadcast/files/S98wfb $(TARGET_DIR)/etc/init.d

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp ../general/package/wifibroadcast/files/telemetry $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp ../general/package/wifibroadcast/files/telemetry.conf $(TARGET_DIR)/etc
endef

$(eval $(generic-package))
