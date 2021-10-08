################################################################################
#
# microbe-web | updated 2021.10.08
#
################################################################################

MICROBE_WEB_VERSION = 28201f3534d5c2a7e4157105b31c359a5d0e4213
MICROBE_WEB_SITE = $(call github,openipc,microbe-web,$(MICROBE_WEB_VERSION))
MICROBE_WEB_LICENSE = MIT
MICROBE_WEB_LICENSE_FILES = LICENSE

define MICROBE_WEB_INSTALL_TARGET_CMDS
	cp -rv $(@D)/files_old/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
