################################################################################
#
# linux-patcher
#
################################################################################

ifneq ($(BR2_PACKAGE_WIRELESS_TOOLS),y)
define LINUX_PATCHER_CONFIG_CFG80211
	$(call KCONFIG_DISABLE_OPT,CONFIG_CFG80211)
endef
endif

define LINUX_PATCHER_LINUX_CONFIG_FIXUPS
	$(LINUX_PATCHER_CONFIG_CFG80211)
endef

$(eval $(generic-package))
