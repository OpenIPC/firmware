################################################################################
#
# wifibroadcast-ext
#
################################################################################

define WIFIBROADCAST_EXT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIFIBROADCAST_EXT_PKGDIR)/files/S75generate_vtx_info

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(WIFIBROADCAST_EXT_PKGDIR)/files/generate_vtx_info
endef

$(eval $(generic-package))
