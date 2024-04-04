RUBYFPV_SITE_METHOD = git
RUBYFPV_SITE = https://github.com/PetruSoroaga/RubyFPV
RUBYFPV_VERSION = ae681b04cef5b45f4f95f2d3595b293f1fe4aa5a

RUBYFPV_DEPENDENCIES += libpcap iw

RUBYFPV_MAKE_OPTS = \
    CC="$(TARGET_CC)" \
    CXX="$(TARGET_CXX)"

define RUBYFPV_BUILD_CMDS
	$(MAKE) $(RUBYFPV_MAKE_OPTS) -C $(@D) vehicle RUBY_BUILD_ENV=openipc
endef

define RUBYFPV_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(RUBYFPV_PKGDIR)/files/S73ruby

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/ruby_start
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/ruby_logger
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/ruby_rt_vehicle
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/ruby_tx_telemetry
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(RUBYFPV_PKGDIR)/files/ruby_stop.sh
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/sbin $(RUBYFPV_PKGDIR)/files/version_ruby_base.txt

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(RUBYFPV_PKGDIR)/files/tweaksys

	$(INSTALL) -m 755 -d $(TARGET_DIR)/root/ruby/licenses
	$(INSTALL) -m 644 -t $(TARGET_DIR)/root/ruby/licenses $(@D)/licenses/*
endef

$(eval $(generic-package))
