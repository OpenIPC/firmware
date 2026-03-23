################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# ntfy — lightweight ntfy.sh notification client
#
################################################################################

NTFY_LICENSE       = MIT
NTFY_LICENSE_FILES = README.md
NTFY_DEPENDENCIES  = libcurl-openipc

define NTFY_EXTRACT_CMDS
	cp -avr $(NTFY_PKGDIR)/src/* $(@D)/
endef

define NTFY_BUILD_CMDS
	$(MAKE) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) -Wl,--gc-sections -Wl,--strip-all" \
		LIBS="-lcurl" \
		-C $(@D)
endef

define NTFY_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 $(@D)/ntfy $(TARGET_DIR)/usr/bin/ntfy
endef

$(eval $(generic-package))
