################################################################################
#
# microbe-web
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
MICROBE_WEB_SITE_METHOD = git
MICROBE_WEB_SITE = https://github.com/openipc/microbe-web
MICROBE_WEB_VERSION = $(shell git ls-remote $(MICROBE_WEB_SITE) HEAD | head -1 | cut -f1)
else
MICROBE_WEB_SITE = https://github.com/openipc/microbe-web/archive
MICROBE_WEB_SOURCE = master.tar.gz
endif

MICROBE_WEB_LICENSE = MIT
MICROBE_WEB_LICENSE_FILES = LICENSE

define MICROBE_WEB_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp $(MICROBE_WEB_PKGDIR)/files/httpd.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(MICROBE_WEB_PKGDIR)/files/S50httpd $(TARGET_DIR)/etc/init.d
	cp -rv $(@D)/files/etc/init.d/* $(TARGET_DIR)/etc/init.d

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr
	cp -rv $(@D)/files/usr/sbin $(TARGET_DIR)/usr

	$(INSTALL) -m 755 -d $(TARGET_DIR)/var
	cp -rv $(@D)/files/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
