#############################################################
#
# librem-openipc
#
#############################################################

LIBREM_OPENIPC_SITE = $(call github,baresip,rem,$(LIBREM_OPENIPC_VERSION))
LIBREM_OPENIPC_VERSION = v2.0.1

LIBREM_OPENIPC_INSTALL_STAGING = YES
LIBREM_OPENIPC_DEPENDENCIES = libre-openipc

define LIBREM_OPENIPC_BUILD_CMDS
$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) \
	LIBRE_MK=$(STAGING_DIR)/usr/share/re/re.mk \
	LIBRE_INC=$(STAGING_DIR)/usr/include/re \
	LIBRE_SO=$(STAGING_DIR)/usr/lib \
	HAVE_LIBRESOLV= \
	CC="$(TARGET_CC)" \
	EXTRA_CFLAGS="$(TARGET_CFLAGS) -D_GNU_SOURCE" \
	EXTRA_LFLAGS="-lm" \
	DESTDIR="$(STAGING_DIR)" \
	SYSROOT="$(STAGING_DIR)/usr" \
	SYSROOT_ALT="$(STAGING_DIR)/usr" \
	RELEASE=1 \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	OS=linux \
	all install
endef

define LIBREM_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 644 -D $(@D)/librem.so $(TARGET_DIR)/usr/lib/librem.so
	ln -sf librem.so $(TARGET_DIR)/usr/lib/librem.so.2
endef

define LIBREM_OPENIPC_UNINSTALL_STAGING_CMDS
	$(RM) -r $(STAGING_DIR)/usr/include/rem
	$(RM) $(STAGING_DIR)/usr/lib/librem.a
	$(RM) $(STAGING_DIR)/usr/lib/librem.so
endef

define LIBREM_OPENIPC_UNINSTALL_TARGET_CMDS
	$(RM) $(TARGET_DIR)/usr/lib/librem.so
endef

$(eval $(generic-package))
