#############################################################
#
# libre-openipc
#
#############################################################

LIBRE_OPENIPC_SITE = $(call github,baresip,re,$(LIBRE_OPENIPC_VERSION))
LIBRE_OPENIPC_VERSION = v2.1.1

LIBRE_OPENIPC_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_OPENSSL),y)
LIBRE_OPENIPC_DEPENDENCIES += openssl
endif

ifeq ($(BR2_PACKAGE_ZLIB),y)
LIBRE_OPENIPC_DEPENDENCIES += zlib
endif

define LIBRE_OPENIPC_BUILD_CMDS
$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) \
	LIBRE_MK=$(STAGING_DIR)/usr/share/re/re.mk \
	LIBRE_INC=$(STAGING_DIR)/usr/include/re \
	LIBRE_SO=$(STAGING_DIR)/usr/lib \
	CC="$(TARGET_CC)" \
	EXTRA_CFLAGS="$(TARGET_CFLAGS) -D_GNU_SOURCE -fPIC -DOPENBSD" \
	EXTRA_LFLAGS="-lm" \
	DESTDIR="$(STAGING_DIR)" \
	SYSROOT="$(STAGING_DIR)/usr" \
	SYSROOT_ALT="$(STAGING_DIR)/usr" \
	RELEASE=1 \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	OS=linux \
	all install
endef

define LIBRE_OPENIPC_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 644 $(@D)/include/re.h $(STAGING_DIR)/usr/include/re/re.h
endef

define LIBRE_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 644 -D $(@D)/libre.so $(TARGET_DIR)/usr/lib/libre.so
	ln -sf libre.so $(TARGET_DIR)/usr/lib/libre.so.2
endef

define LIBRE_OPENIPC_UNINSTALL_STAGING_CMDS
	$(RM) -r $(STAGING_DIR)/usr/include/re
	$(RM) $(STAGING_DIR)/usr/lib/libre.a
	$(RM) $(STAGING_DIR)/usr/lib/libre.so
endef

define LIBRE_OPENIPC_UNINSTALL_TARGET_CMDS
	$(RM) $(TARGET_DIR)/usr/lib/libre.so
endef

$(eval $(generic-package))
$(eval $(host-generic-package))
