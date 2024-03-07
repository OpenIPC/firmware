################################################################################
#
# majestic-webui
#
################################################################################

MAJESTIC_WEBUI_SITE = $(call github,openipc,majestic-webui,$(MAJESTIC_WEBUI_VERSION))
MAJESTIC_WEBUI_VERSION = HEAD
MAJESTIC_WEBUI_LICENSE = MIT
MAJESTIC_WEBUI_LICENSE_FILES = LICENSE

define MAJESTIC_WEBUI_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr
	cp -r $(@D)/sbin $(TARGET_DIR)/usr

	$(INSTALL) -m 755 -d $(TARGET_DIR)/var
	cp -r $(@D)/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
