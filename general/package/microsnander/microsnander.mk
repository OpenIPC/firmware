################################################################################
#
# microsnander
#
################################################################################

MICROSNANDER_SITE = $(call github,openipc,microsnander,$(MICROSNANDER_VERSION))
MICROSNANDER_VERSION = master

MICROSNANDER_LICENSE = MIT
MICROSNANDER_LICENSE_FILES = LICENSE

define MICROSNANDER_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/src
endef

define MICROSNANDER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/src/microsnander $(TARGET_DIR)/usr/sbin/microsnander
endef

$(eval $(generic-package))
