################################################################################
#
# webui-next
#
################################################################################

WEBUI_NEXT_SITE = $(call github,openipc,webui-next,$(WEBUI_NEXT_VERSION))
WEBUI_NEXT_VERSION = master
WEBUI_NEXT_LICENSE = MIT
WEBUI_NEXT_LICENSE_FILES = LICENSE

define WEBUI_NEXT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr
	cp -r $(@D)/sbin $(TARGET_DIR)/usr

	$(INSTALL) -m 755 -d $(TARGET_DIR)/var
	cp -r $(@D)/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
