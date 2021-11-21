################################################################################
#
# microbe-web | updated 2021.11.21
#
################################################################################

MICROBE_WEB_VERSION = 09c6b2b9702b909daa6a27d2514dddb22383acbf
MICROBE_WEB_SITE = $(call github,openipc,microbe-web,$(MICROBE_WEB_VERSION))
MICROBE_WEB_LICENSE = MIT
MICROBE_WEB_LICENSE_FILES = LICENSE

define MICROBE_WEB_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp ../general/package/microbe-web/files/httpd.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp ../general/package/microbe-web/files/S50httpd $(TARGET_DIR)/etc/init.d

	$(INSTALL) -m 755 -d $(TARGET_DIR)/var
	cp -rv $(@D)/files/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
