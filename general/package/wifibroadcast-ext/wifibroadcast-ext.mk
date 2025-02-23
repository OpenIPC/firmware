################################################################################
#
# wifibroadcast-ext
#
################################################################################

WIFIBROADCAST_EXT_DEPENDENCIES += libsodium

define WIFIBROADCAST_EXT_BUILD_CMDS
	$(TARGET_CC) $(WIFIBROADCAST_EXT_PKGDIR)/src/keygen.c -o $(@D)/keygen -lsodium -s
endef

define WIFIBROADCAST_EXT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_EXT_PKGDIR)/files/S75generate_vtx_info

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/keygen
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/generate_vtx_info.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/bitrate_calculator.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/channels.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/generate_backup.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/provision_bind.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/provision_listen.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/set_bitrate.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/set_live_bitrate.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/set_live_tx.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/temperature_check.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/wifibroadcast

  ##/etc/
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_EXT_PKGDIR)/files/files_to_backup.txt
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_EXT_PKGDIR)/files/rc.local
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_EXT_PKGDIR)/files/vtx_profiles.yaml
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_EXT_PKGDIR)/files/wifi_profiles.yaml
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_EXT_PKGDIR)/files/wfb.yaml

  ##/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_EXT_PKGDIR)/files/S75generate_vtx_info
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_EXT_PKGDIR)/files/S98wifibroadcast
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_EXT_PKGDIR)/files/S990provisioner_bind
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_EXT_PKGDIR)/files/S992alink
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_EXT_PKGDIR)/files/S993temperature_check
endef

$(eval $(generic-package))
