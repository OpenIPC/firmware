################################################################################
#
# linux-patcher
#
################################################################################

export UIMAGE_NAME = Linux-$(LINUX_VERSION_PROBED)-$(OPENIPC_SOC_MODEL)

# Exclude buildroot yylloc patches
LINUX_POST_PATCH_HOOKS = LINUX_APPLY_LOCAL_PATCHES

ifeq ($(BR2_PACKAGE_LINUX_PATCHER_ATHEROS),y)
define LINUX_PATCHER_CONFIG_ATHEROS
	$(call KCONFIG_ENABLE_OPT,CONFIG_FW_LOADER)
	$(call KCONFIG_ENABLE_OPT,CONFIG_WLAN)
	$(call KCONFIG_SET_OPT,CONFIG_MAC80211,m)
	$(call KCONFIG_SET_OPT,CONFIG_ATH9K,m)
	$(call KCONFIG_SET_OPT,CONFIG_ATH9K_HTC,m)
endef
endif

define LINUX_PATCHER_LINUX_CONFIG_FIXUPS
	$(LINUX_PATCHER_CONFIG_ATHEROS)
endef

$(eval $(generic-package))
