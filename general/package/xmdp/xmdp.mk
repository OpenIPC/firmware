################################################################################
#
# xmdp
#
################################################################################

XMDP_LICENSE = Public Domain

define XMDP_EXTRACT_CMDS
	cp package/x11r7/mcookie/mcookie.c $(@D)/
endef

define XMDP_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -Wall -Os -s mcookie.c -o mcookie)
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -O -g -D LINUX -std=c99 -Wall -Wpedantic -lm -o $(PKG_BUILD_DIR)/$(PKG_NAME) \
	$(PKG_BUILD_DIR)/xmdp.c $(PKG_BUILD_DIR)/netip.c $(PKG_BUILD_DIR)/utils.c $(PKG_BUILD_DIR)/cjson/cJSON.c
endef

define XMDP_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/xmdp $(TARGET_DIR)/usr/bin/xmdp
endef

$(eval $(generic-package))
