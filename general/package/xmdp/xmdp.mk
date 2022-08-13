################################################################################
#
# xmdp | updated 2022.08.10
#
################################################################################

XMDP_LICENSE = Public Domain

define XMDP_EXTRACT_CMDS
	cp -avr ../general/package/xmdp/src/* $(@D)/
endef

#define XMDP_BUILD_CMDS
#	(cd $(@D); $(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -O -g -D LINUX -std=c99 -Wall -Wpedantic -lm -o $(PKG_NAME) xmdp.c netip.c utils.c cjson/cJSON.c)
#endef

define XMDP_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)
endef


define XMDP_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/xmdp $(TARGET_DIR)/usr/bin/xmdp
endef

$(eval $(generic-package))
