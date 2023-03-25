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
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(WIRELESS_CONFIGURATION_PATH)/udhcpd.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/wireless
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/wireless $(WIRELESS_CONFIGURATION_PATH)/interfaces
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/wireless $(WIRELESS_CONFIGURATION_PATH)/netadapter
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/wireless $(WIRELESS_CONFIGURATION_PATH)/netsetup

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(WIRELESS_CONFIGURATION_PATH)/wireless
endef

$(eval $(generic-package))
