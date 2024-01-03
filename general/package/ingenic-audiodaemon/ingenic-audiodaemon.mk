################################################################################
#
# ingenic-audiodaemon
#
################################################################################

INGENIC_AUDIODAEMON_VERSION = master
INGENIC_AUDIODAEMON_SITE = $(call github,gtxaspec,ingenic_audiodaemon,$(INGENIC_AUDIODAEMON_VERSION))
INGENIC_AUDIODAEMON_LICENSE = GPL-2.0
INGENIC_AUDIODAEMON_LICENSE_FILES = COPYING

define INGENIC_AUDIODAEMON_BUILD_CMDS
    $(MAKE) CROSS_COMPILE=$(TARGET_CROSS) deps -C $(@D)
    $(MAKE) CROSS_COMPILE=$(TARGET_CROSS) all -C $(@D)
endef

define INGENIC_AUDIODAEMON_INSTALL_TARGET_CMDS
    cp -a $(@D)/build/bin/. $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
