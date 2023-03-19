################################################################################
#
# wireless-configuration
#
################################################################################

WIRELESS_CONFIGURATION_VERSION =
WIRELESS_CONFIGURATION_SITE =
WIRELESS_CONFIGURATION_LICENSE = MIT
WIRELESS_CONFIGURATION_LICENSE_FILES = LICENSE

WIRELESS_CONFIGURATION_PATH = ../general/package/wireless-configuration/files

define WIRELESS_CONFIGURATION_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp -f $(WIRELESS_CONFIGURATION_PATH)/udhcpd.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/wireless
	cp -f $(WIRELESS_CONFIGURATION_PATH)/interfaces $(TARGET_DIR)/etc/wireless
	cp -f $(WIRELESS_CONFIGURATION_PATH)/netadapter $(TARGET_DIR)/etc/wireless
	cp -f $(WIRELESS_CONFIGURATION_PATH)/netsetup $(TARGET_DIR)/etc/wireless

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	cp -f $(WIRELESS_CONFIGURATION_PATH)/wireless $(TARGET_DIR)/usr/sbin
endef

$(eval $(generic-package))
