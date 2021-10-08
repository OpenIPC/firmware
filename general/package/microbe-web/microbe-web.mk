################################################################################
#
# microbe-web | updated 2021.10.08
#
################################################################################

MICROBE_WEB_VERSION = b7dc70d21453d5bb9890119df914148bcb615d04
MICROBE_WEB_SITE = $(call github,openipc,microbe-web,$(MICROBE_WEB_VERSION))
MICROBE_WEB_LICENSE = MIT
MICROBE_WEB_LICENSE_FILES = LICENSE

define MICROBE_WEB_INSTALL_TARGET_CMDS
	cp -rv $(@D)/files_old/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
