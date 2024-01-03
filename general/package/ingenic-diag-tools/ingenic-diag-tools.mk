################################################################################
#
# ingenic-diag-tools
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
INGENIC_DIAG_TOOLS_SITE_METHOD = git
INGENIC_DIAG_TOOLS_SITE = https://github.com/gtxaspec/jz-diag-tools
INGENIC_DIAG_TOOLS_VERSION = $(shell git ls-remote $(INGENIC_DIAG_TOOLS_SITE) HEAD | head -1 | cut -f1)
else
INGENIC_DIAG_TOOLS_SITE = https://github.com/gtxaspec/jz-diag-tools/archive
INGENIC_DIAG_TOOLS_SOURCE = main.tar.gz
endif

define INGENIC_DIAG_TOOLS_BUILD_CMDS
    $(MAKE) CROSS_COMPILE=$(TARGET_CROSS) -C $(@D)
endef

define INGENIC_DIAG_TOOLS_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/ingenic-gpio $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
