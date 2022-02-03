################################################################################
#
# microbe-web
#
################################################################################

MICROBE_WEB_VERSION = HEAD
MICROBE_WEB_SITE = $(call github,openipc,microbe-web,$(MICROBE_WEB_VERSION))
MICROBE_WEB_LICENSE = MIT
MICROBE_WEB_LICENSE_FILES = LICENSE

define MICROBE_WEB_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp ../general/package/microbe-web/files/httpd.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp ../general/package/microbe-web/files/S50httpd $(TARGET_DIR)/etc/init.d

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr
	cp -rv $(@D)/files/usr/sbin $(TARGET_DIR)/usr

	$(INSTALL) -m 755 -d $(TARGET_DIR)/var
	cp -rv $(@D)/files/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
