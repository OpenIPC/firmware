################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# herald — compact mDNS/DNS-SD stack (RFC 6762 / RFC 6763)
#
################################################################################

HERALD_LICENSE = MIT
HERALD_LICENSE_FILES = README.md

define HERALD_EXTRACT_CMDS
	cp -avr $(HERALD_PKGDIR)/src/* $(@D)/
endef

define HERALD_BUILD_CMDS
	$(MAKE) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) -Wl,--gc-sections -Wl,--strip-all" \
		ARCH_FLAGS="" \
		-C $(@D)
endef

define HERALD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 $(@D)/herald-client $(TARGET_DIR)/usr/sbin/herald-client
	$(INSTALL) -m 755 $(@D)/herald-server  $(TARGET_DIR)/usr/sbin/herald-server
endef

$(eval $(generic-package))
