################################################################################
#
# majestic-plugins
#
################################################################################

MAJESTIC_PLUGINS_SITE = https://github.com/openipc/majestic-plugins/archive
MAJESTIC_PLUGINS_SOURCE = master.tar.gz

MAJESTIC_PLUGINS_PATH = $(BASE_DIR)/../majestic-plugins

ifneq ($(wildcard $(MAJESTIC_PLUGINS_PATH)),)
define MAJESTIC_PLUGINS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) TARGET=$(OPENIPC_SOC_VENDOR) -C $(MAJESTIC_PLUGINS_PATH) -B
endef
else
define MAJESTIC_PLUGINS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) TARGET=$(OPENIPC_SOC_VENDOR) -C $(@D) -B
endef

define MAJESTIC_PLUGINS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(@D)/$(OPENIPC_SOC_VENDOR).so
endef
endif

$(eval $(generic-package))
