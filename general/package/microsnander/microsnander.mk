################################################################################
#
# microsnander | updated 2022.08.13
#
################################################################################

MICROSNANDER_VERSION = df9cc51c27a84c2d7eee8d90b7586a11d97c7ff2
MICROSNANDER_SITE = $(call github,openipc,microsnander,$(MICROSNANDER_VERSION))
MICROSNANDER_LICENSE = MIT
MICROSNANDER_LICENSE_FILES = LICENSE

define MICROSNANDER_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/src
endef

define MICROSNANDER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/src/snander $(TARGET_DIR)/usr/sbin/microsnander
endef

$(eval $(generic-package))
