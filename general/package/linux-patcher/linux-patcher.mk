################################################################################
#
# linux-patcher
#
################################################################################

LINUX_PATCHER_DEPENDENCIES = linux
# The release without a vendor's "__..." local version: the Ingenic 3.10
# kernels' (3.10.14__isvp_swan_1.0__) ran the name past the uImage header's
# 32 bytes, cut to "...-t", where sysupgrade reads the SoC. The probe is a
# command the shell runs when mkimage does, so the cut goes into it.
export UIMAGE_NAME = Linux-`$(subst `,,$(LINUX_VERSION_PROBED)) | sed 's/__.*//'`-$(OPENIPC_SOC_MODEL)

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

ifneq ($(BR2_PACKAGE_LINUX_PATCHER_SIGMASTAR_DTB),"")
define LINUX_PATCHER_CONFIG_SIGMASTAR_DTB
	$(call KCONFIG_SET_OPT,CONFIG_SS_DTB_NAME,$(BR2_PACKAGE_LINUX_PATCHER_SIGMASTAR_DTB))
endef
endif

define LINUX_PATCHER_LINUX_CONFIG_FIXUPS
	$(LINUX_PATCHER_CONFIG_ATHEROS)
	$(LINUX_PATCHER_CONFIG_SIGMASTAR_DTB)
endef

$(eval $(generic-package))
