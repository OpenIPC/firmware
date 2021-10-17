################################################################################
#
# microbe-web | updated 2021.10.17
#
################################################################################

MICROBE_WEB_VERSION = 2738a4aa81650b2b45787241efad4fb40af3d143
MICROBE_WEB_SITE = $(call github,openipc,microbe-web,$(MICROBE_WEB_VERSION))
MICROBE_WEB_LICENSE = MIT
MICROBE_WEB_LICENSE_FILES = LICENSE

define MICROBE_WEB_INSTALL_TARGET_CMDS
	cp -rv $(@D)/files/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
