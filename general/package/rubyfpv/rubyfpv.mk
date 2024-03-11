################################################################################
#
# rubyfpv
#
################################################################################

RUBYFPV_SITE = $(call github,petrusoroaga,rubyfpv,$(RUBYFPV_VERSION))
RUBYFPV_VERSION = HEAD

RUBYFPV_DEPENDENCIES = libpcap libsodium
RUBYFPV_TARGET = ruby_start ruby_logger ruby_rx_commands ruby_tx_telemetry ruby_rt_vehicle

define RUBYFPV_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) $(RUBYFPV_TARGET)
endef

define RUBYFPV_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(RUBYFPV_PKGDIR)/files/S73ruby

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/ruby
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/ruby $(@D)/licenses/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(RUBYFPV_PKGDIR)/files/tweaksys

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/ruby*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(RUBYFPV_PKGDIR)/files/ruby_stop
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/sbin $(RUBYFPV_PKGDIR)/files/version_ruby_base.txt
endef

$(eval $(generic-package))
