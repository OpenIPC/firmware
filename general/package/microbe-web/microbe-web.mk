################################################################################
#
# microbe-web | updated 2021.11.09
#
################################################################################

MICROBE_WEB_VERSION = 09c6b2b9702b909daa6a27d2514dddb22383acbf
MICROBE_WEB_SITE = $(call github,openipc,microbe-web,$(MICROBE_WEB_VERSION))
MICROBE_WEB_LICENSE = MIT
MICROBE_WEB_LICENSE_FILES = LICENSE

define MICROBE_WEB_INSTALL_TARGET_CMDS
	cp -rv $(@D)/files/var/www $(TARGET_DIR)/var
endef

$(eval $(generic-package))
