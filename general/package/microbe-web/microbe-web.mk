################################################################################
#
# microbe-web | updated 2021.10.17
#
################################################################################

MICROBE_WEB_VERSION = 7203cf1eaeddbab7453e9f02da0a539428cc0f73
MICROBE_WEB_SITE = $(call github,openipc,microbe-web,$(MICROBE_WEB_VERSION))
MICROBE_WEB_LICENSE = MIT
MICROBE_WEB_LICENSE_FILES = LICENSE

define MICROBE_WEB_INSTALL_TARGET_CMDS
	cp -rv $(@D)/files/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
