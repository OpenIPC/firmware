RUBYFPV_SITE_METHOD = git
RUBYFPV_SITE = https://github.com/PetruSoroaga/RubyFPV
RUBYFPV_VERSION = dca5c9ff9ca9a06bbf5289639f0a6e0715f77471

RUBYFPV_DEPENDENCIES += libpcap iw

RUBYFPV_MAKE_OPTS = \
    CC="$(TARGET_CC)" \
    CXX="$(TARGET_CXX)"

define RUBYFPV_EXTRACT_CMDS
	cd $(RUBYFPV_DL_DIR); tar -xf rubyfpv-$(RUBYFPV_VERSION)-br1.tar.gz -S
	cp -avr $(RUBYFPV_DL_DIR)/rubyfpv-$(RUBYFPV_VERSION)/* $(@D)
	cd $(@D); cp -avr $(RUBYFPV_PKGDIR)/files/* $(@D)/
endef


define RUBYFPV_BUILD_CMDS
	$(MAKE) $(RUBYFPV_MAKE_OPTS) -C $(@D) vehicle RUBY_BUILD_ENV=openipc
endef

define RUBYFPV_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -d $(TARGET_DIR)/root/ruby/licenses
 
	cp $(RUBYFPV_PKGDIR)/files/S73ruby $(TARGET_DIR)/etc/init.d
	chmod 755 $(TARGET_DIR)/etc/init.d/S73ruby

	install -m 0755 -D $(@D)/ruby_start $(TARGET_DIR)/usr/sbin/ruby_start
	install -m 0755 -D $(@D)/ruby_logger $(TARGET_DIR)/usr/sbin/ruby_logger
	install -m 0755 -D $(@D)/ruby_rt_vehicle $(TARGET_DIR)/usr/sbin/ruby_rt_vehicle
	install -m 0755 -D $(@D)/ruby_tx_telemetry $(TARGET_DIR)/usr/sbin/ruby_tx_telemetry
	install -m 0755 -D $(@D)/ruby_rx_commands $(TARGET_DIR)/usr/sbin/ruby_rx_commands

	cp $(RUBYFPV_PKGDIR)/files/ruby_stop.sh $(TARGET_DIR)/usr/sbin
	chmod 755 $(TARGET_DIR)/usr/sbin/ruby_stop.sh
	cp $(RUBYFPV_PKGDIR)/files/tweaksys $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 644 -D $(@D)/licenses/* $(TARGET_DIR)/root/ruby/licenses
	cp $(RUBYFPV_PKGDIR)/files/version_ruby_base.txt $(TARGET_DIR)/usr/sbin
endef

$(eval $(generic-package))
