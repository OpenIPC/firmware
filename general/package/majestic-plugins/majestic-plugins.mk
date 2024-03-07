################################################################################
#
# majestic-plugins
#
################################################################################

MAJESTIC_PLUGINS_SITE = $(call github,openipc,majestic-plugins,$(MAJESTIC_PLUGINS_VERSION))
MAJESTIC_PLUGINS_VERSION = HEAD

define MAJESTIC_PLUGINS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) TARGET=$(OPENIPC_SOC_VENDOR) -C $(@D) -B
endef

define MAJESTIC_PLUGINS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(@D)/$(OPENIPC_SOC_VENDOR).so
endef

$(eval $(generic-package))
