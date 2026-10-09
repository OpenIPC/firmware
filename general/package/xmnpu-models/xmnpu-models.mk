################################################################################
#
# xmnpu-models
#
################################################################################

# Built and published by that repository's release workflow, never by hand:
# each asset traces to the commit and run that compiled it.
XMNPU_MODELS_VERSION = v3
XMNPU_MODELS_SITE = https://github.com/OpenIPC/xmnpu-models/releases/download/$(XMNPU_MODELS_VERSION)
XMNPU_MODELS_SOURCE = yolov8n.xmm
XMNPU_MODELS_EXTRA_DOWNLOADS = yolov8s.xmm
XMNPU_MODELS_LICENSE = AGPL-3.0
XMNPU_MODELS_INSTALL_TARGET = NO

define XMNPU_MODELS_EXTRACT_CMDS
	cp $(addprefix $(XMNPU_MODELS_DL_DIR)/,$(XMNPU_MODELS_SOURCE) $(XMNPU_MODELS_EXTRA_DOWNLOADS)) $(@D)/
endef

# Into the UBIFS image's own copy of the target tree only, the same way the
# NAND kernel goes to /boot (general/external.mk): the models are 22 MB, which
# the NOR squashfs built from the same tree has no room for.
define XMNPU_MODELS_UBIFS_INSTALL
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/majestic/npu
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/majestic/npu \
		$(XMNPU_MODELS_BUILDDIR)/yolov8n.xmm $(XMNPU_MODELS_BUILDDIR)/yolov8s.xmm
endef

ifeq ($(BR2_PACKAGE_XMNPU_MODELS),y)
ROOTFS_UBIFS_PRE_GEN_HOOKS += XMNPU_MODELS_UBIFS_INSTALL
ROOTFS_UBIFS_DEPENDENCIES += xmnpu-models
endif

$(eval $(generic-package))
