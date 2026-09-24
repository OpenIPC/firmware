################################################################################
# ap-alink
################################################################################

AP_ALINK_VERSION = 1.2
AP_ALINK_SITE = (call github,carabidulebabat,ap-alink,$(AP_ALINK_VERSION))

define AP_ALINK_BUILD_CMDS
	$(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define AP_ALINK_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/ap-alink $(TARGET_DIR)/usr/bin/ap-alink
	$(INSTALL) -D -m 0644 $(@D)/ap-alink.conf $(TARGET_DIR)/etc/ap-alink.conf
endef

$(eval $(generic-package))
