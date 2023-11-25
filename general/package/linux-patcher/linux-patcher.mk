################################################################################
#
# linux-patcher
#
################################################################################

export UIMAGE_NAME = Linux-$(LINUX_VERSION_PROBED)-$(OPENIPC_SOC_MODEL)

ifneq ($(BR2_PACKAGE_WIRELESS_TOOLS),y)
define LINUX_PATCHER_CONFIG_CFG80211
	$(call KCONFIG_DISABLE_OPT,CONFIG_CFG80211)
endef
endif

ifeq ($(BOARD),ssc338q_fpv)
define LINUX_PATCHER_CONFIG_ATHEROS
	$(call KCONFIG_ENABLE_OPT,CONFIG_FW_LOADER)
	$(call KCONFIG_ENABLE_OPT,CONFIG_WLAN)
	$(call KCONFIG_SET_OPT,CONFIG_MAC80211,m)
	$(call KCONFIG_SET_OPT,CONFIG_ATH9K,m)
	$(call KCONFIG_SET_OPT,CONFIG_ATH9K_HTC,m)
endef
endif

define LINUX_PATCHER_LINUX_CONFIG_FIXUPS
	$(LINUX_PATCHER_CONFIG_CFG80211)
	$(LINUX_PATCHER_CONFIG_ATHEROS)
endef

$(eval $(generic-package))
