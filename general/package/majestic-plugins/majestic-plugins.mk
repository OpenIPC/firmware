################################################################################
#
# majestic-plugins
#
################################################################################

MAJESTIC_PLUGINS_SITE_METHOD = git
MAJESTIC_PLUGINS_SITE = https://github.com/openipc/majestic-plugins
MAJESTIC_PLUGINS_VERSION = HEAD

MAJESTIC_PLUGINS_GIT_SUBMODULES = YES

define MAJESTIC_PLUGINS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) TARGET=$(OPENIPC_SOC_VENDOR) -C $(@D) -B
endef

define MAJESTIC_PLUGINS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(@D)/$(OPENIPC_SOC_VENDOR).so
endef

$(eval $(generic-package))
