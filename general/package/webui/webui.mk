################################################################################
#
# webui
#
################################################################################

WEBUI_SITE = $(call github,openipc,webui,$(WEBUI_VERSION))
WEBUI_VERSION = master

WEBUI_LICENSE = MIT
WEBUI_LICENSE_FILES = LICENSE

define WEBUI_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(WEBUI_PKGDIR)/files/etc/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(WEBUI_PKGDIR)/files/init.d/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr
	cp -rv $(@D)/files/usr/sbin $(TARGET_DIR)/usr

	$(INSTALL) -m 755 -d $(TARGET_DIR)/var
	cp -rv $(@D)/files/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
