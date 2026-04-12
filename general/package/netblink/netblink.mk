################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# netblink — unified network LED indicator: ping quality or traffic activity
#
################################################################################

NETBLINK_LICENSE       = MIT
NETBLINK_LICENSE_FILES = README.md

define NETBLINK_EXTRACT_CMDS
	cp -avr $(NETBLINK_PKGDIR)/src/* $(@D)/
endef

define NETBLINK_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall -Wextra -Os -std=c99 -D_XOPEN_SOURCE=600" \
		LDFLAGS="$(TARGET_LDFLAGS) -s" \
		-C $(@D)
endef

define NETBLINK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	install -m 0755 -D $(@D)/netblink $(TARGET_DIR)/usr/sbin/netblink
endef

$(eval $(generic-package))
