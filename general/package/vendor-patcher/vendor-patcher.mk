################################################################################
#
# vendor-patcher
#
################################################################################

VENDOR_PATCHER_PATH = $(BR2_EXTERNAL_VENDOR)/board/$(EXTERNAL_FAMILY)/kernel
LINUX_PRE_PATCH_HOOKS += VENDOR_PATCHER_LOCAL_FILES

ifneq ($(BR2_PACKAGE_WIRELESS_TOOLS),y)
define VENDOR_PATCHER_CONFIG_CFG80211
	$(call KCONFIG_DISABLE_OPT,CONFIG_CFG80211)
endef
endif

define VENDOR_PATCHER_LINUX_CONFIG_FIXUPS
	$(VENDOR_PATCHER_CONFIG_CFG80211)
endef

define VENDOR_PATCHER_LOCAL_FILES
	# Workaround for buildroot 2021
	$(VENDOR_PATCHER_PKGDIR)/apply-patches.sh $(LINUX_DIR) $(VENDOR_PATCHER_PATH)/patches

	#$(APPLY_PATCHES) $(LINUX_DIR) $(VENDOR_PATCHER_PATH)/patches
	if test -e $(VENDOR_PATCHER_PATH)/overlay; then \
		cp -rf $(VENDOR_PATCHER_PATH)/overlay/* $(LINUX_DIR); fi
endef

$(eval $(generic-package))
