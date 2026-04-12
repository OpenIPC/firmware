################################################################################
#
# Adaptive Link
#
################################################################################

ADAPTIVE_LINK_SITE = $(call github,openipc,adaptive-link,$(ADAPTIVE_LINK_VERSION))
ADAPTIVE_LINK_VERSION = '3a831a75cb25df403374fa5104ea494c140695da'
ADAPTIVE_LINK_DEPENDENCIES = yaml-cli-multi

ADAPTIVE_LINK_LICENSE = GPL-3.0
ADAPTIVE_LINK_LICENSE_FILES = LICENSE


ifeq ($(BR2_TOOLCHAIN_USES_GLIBC),y)
	ADAPTIVE_LINK_OPTIONS = "-rdynamic -s -Os -lm"
else
	ADAPTIVE_LINK_OPTIONS = "-rdynamic -s -Os"
endif

define ADAPTIVE_LINK_BUILD_CMDS
	@echo "Building Adaptive link"
	$(MAKE) CC=$(TARGET_CC) OPT=$(ADAPTIVE_LINK_OPTIONS) -C $(@D)
endef

define ADAPTIVE_LINK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/alink.conf
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/wlan_adapters.yaml
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/txprofiles/txprofiles.conf
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/alink_drone
endef

$(eval $(generic-package))
