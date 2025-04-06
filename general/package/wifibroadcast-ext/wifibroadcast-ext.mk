################################################################################
#
# wifibroadcast-ext
#
################################################################################

define WIFIBROADCAST_EXT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(WIFIBROADCAST_NG_PKGDIR)/files/wfb.yaml

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_EXT_PKGDIR)/files/S75generate_vtx_info
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_NG_PKGDIR)/files/S991temp_monitor

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/generate_vtx_info
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_NG_PKGDIR)/files/wifibroadcast
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_NG_PKGDIR)/files/temp_monitor
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_NG_PKGDIR)/files/channels.sh
endef

$(eval $(generic-package))
