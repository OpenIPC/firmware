################################################################################
#
# fake-hwclock
#
################################################################################

define FAKE_HWCLOCK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(FAKE_HWCLOCK_PKGDIR)/files/S39fakehwclock

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(FAKE_HWCLOCK_PKGDIR)/files/fake-hwclock
endef

$(eval $(generic-package))
