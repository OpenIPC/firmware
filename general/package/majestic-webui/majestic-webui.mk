################################################################################
#
# majestic-webui
#
################################################################################

MAJESTIC_WEBUI_SITE = $(call github,openipc,majestic-webui,$(MAJESTIC_WEBUI_VERSION))
MAJESTIC_WEBUI_VERSION = HEAD
MAJESTIC_WEBUI_LICENSE = MIT
MAJESTIC_WEBUI_LICENSE_FILES = LICENSE

ifneq ($(findstring fpv,$(OPENIPC_VARIANT)),)
	VERSION = FPV
else
	VERSION = STANDARD
endif

define MAJESTIC_WEBUI_INSTALL
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr
	cp -r $(@D)/sbin $(TARGET_DIR)/usr

	$(INSTALL) -m 755 -d $(TARGET_DIR)/var
	cp -r $(@D)/www $(TARGET_DIR)/var
endef

define MAJESTIC_WEBUI_STANDARD_FIXUP
	rm $(TARGET_DIR)/var/www/cgi-bin/fpv-wfb.cgi
	rm $(TARGET_DIR)/var/www/cgi-bin/j/locale_fpv.cgi
	rm $(TARGET_DIR)/var/www/cgi-bin/p/header_fpv.cgi
	rm $(TARGET_DIR)/var/www/cgi-bin/p/fpv_common.cgi
endef

define MAJESTIC_WEBUI_FPV_FIXUP
	mv -f $(TARGET_DIR)/var/www/cgi-bin/j/locale_fpv.cgi $(TARGET_DIR)/var/www/cgi-bin/j/locale.cgi
	mv -f $(TARGET_DIR)/var/www/cgi-bin/p/header_fpv.cgi $(TARGET_DIR)/var/www/cgi-bin/p/header.cgi
	rm $(TARGET_DIR)/usr/sbin/telegram
	rm $(TARGET_DIR)/usr/sbin/openwall
endef

define MAJESTIC_WEBUI_INSTALL_TARGET_CMDS
	$(MAJESTIC_WEBUI_INSTALL)
	$(MAJESTIC_WEBUI_$(VERSION)_FIXUP)
endef

$(eval $(generic-package))
