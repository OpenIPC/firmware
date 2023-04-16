################################################################################
#
# wireless-configuration
#
################################################################################

WIRELESS_CONFIGURATION_VERSION =
WIRELESS_CONFIGURATION_SITE =
WIRELESS_CONFIGURATION_LICENSE = MIT
WIRELESS_CONFIGURATION_LICENSE_FILES = LICENSE

WIRELESS_CONFIGURATION_PATH = $(WIRELESS_CONFIGURATION_PKGDIR)/files

define WIRELESS_CONFIGURATION_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(WIRELESS_CONFIGURATION_PATH)/network/udhcpd.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WIRELESS_CONFIGURATION_PATH)/script/S30wireless

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/network
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/network $(WIRELESS_CONFIGURATION_PATH)/network/interfaces.conf
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/network $(WIRELESS_CONFIGURATION_PATH)/script/adapter

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(WIRELESS_CONFIGURATION_PATH)/script/wireless
endef

$(eval $(generic-package))
