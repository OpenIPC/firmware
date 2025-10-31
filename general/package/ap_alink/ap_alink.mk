################################################################################
# apalink
################################################################################

APALINK_VERSION = 1.2
APALINK_SITE = (call github,carabidulebabat,AP-ALINK-FPV,$(APALINK_VERSION))

define APALINK_BUILD_CMDS
	$(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define APALINK_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/ap_alink $(TARGET_DIR)/usr/bin/ap_alink
	$(INSTALL) -D -m 0644 $(@D)/ap_alink.conf $(TARGET_DIR)/etc/ap_alink.conf
endef

$(eval $(generic-package))
