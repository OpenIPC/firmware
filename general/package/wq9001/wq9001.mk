################################################################################
#
# wq9001
#
################################################################################

WQ9001_SITE = $(call github,gtxaspec,wq9001-wifi,$(WQ9001_VERSION))
WQ9001_VERSION = HEAD
WQ9001_LICENSE = GPL-2.0

define WQ9001_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/wifi
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/wifi $(@D)/cal/WQ9001_WiFi_Mapfile.dat
endef

WQ9001_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
