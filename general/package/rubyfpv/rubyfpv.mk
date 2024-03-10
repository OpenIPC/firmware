################################################################################
#
# rubyfpv
#
################################################################################

RUBYFPV_SITE_METHOD = local
RUBYFPV_SITE = $(RUBYFPV_PKGDIR)/src

define RUBYFPV_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) -C $(@D) all
endef

define RUBYFPV_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(RUBYFPV_PKGDIR)/files/S73ruby

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/ruby
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/ruby $(RUBYFPV_PKGDIR)/files/licences/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(RUBYFPV_PKGDIR)/files/tweaksys

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/output/*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(RUBYFPV_PKGDIR)/files/ruby_stop
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/sbin $(RUBYFPV_PKGDIR)/files/version_ruby_base.txt
endef

$(eval $(generic-package))
