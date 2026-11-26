################################################################################
#
# sigmastar-opensdk
#
################################################################################

SIGMASTAR_OPENSDK_SITE = $(call github,openipc,opensigmastar,$(SIGMASTAR_OPENSDK_VERSION))
SIGMASTAR_OPENSDK_VERSION = 20949c1185e45e1ac4b66927d81cd6d95bfb827f

# The repository is GPL-3.0 and open_hdmi GPL-2.0; the relinked SDK modules
# and the vendor sources built here keep SigmaStar's terms (each module's
# MODULE_LICENSE is the vendor's own).
SIGMASTAR_OPENSDK_LICENSE = GPL-3.0, GPL-2.0 (open_hdmi), PROPRIETARY (SigmaStar objects and sources: open_mhal, open_mi_*, open_fbdev, open_hdmitx)
SIGMASTAR_OPENSDK_LICENSE_FILES = LICENSE

SIGMASTAR_OPENSDK_DEPENDENCIES = linux
SIGMASTAR_OPENSDK_INSTALL_STAGING = YES

# The modules need the kernel's own build tree (drivers/sstar headers), so
# they are built the way the kernel builds out-of-tree modules.
define SIGMASTAR_OPENSDK_BUILD_CMDS
	$(LINUX_MAKE_ENV) $(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) \
		M=$(@D)/kernel CHIPARCH=$(OPENIPC_SOC_FAMILY) modules
endef

# Next to the vendor's modules, where load_sigmastar looks for them.
define SIGMASTAR_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar \
		$(@D)/kernel/*.ko
	$(TARGET_STRIP) --strip-unneeded \
		$(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar/open_*.ko
endef

# MI headers for applications built against the vendor libraries (vigil).
define SIGMASTAR_OPENSDK_INSTALL_STAGING_CMDS
	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/sigmastar
	$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/sigmastar \
		$(@D)/include/$(OPENIPC_SOC_FAMILY)/*.h
endef

$(eval $(generic-package))
