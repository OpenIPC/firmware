################################################################################
#
# ingenic-diag-tools
#
################################################################################

INGENIC_DIAG_TOOLS_SITE = $(call github,gtxaspec,jz-diag-tools,$(INGENIC_DIAG_TOOLS_VERSION))
INGENIC_DIAG_TOOLS_VERSION = HEAD

define INGENIC_DIAG_TOOLS_BUILD_CMDS
    $(MAKE) CROSS_COMPILE=$(TARGET_CROSS) -C $(@D)
endef

define INGENIC_DIAG_TOOLS_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/ingenic-gpio $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
