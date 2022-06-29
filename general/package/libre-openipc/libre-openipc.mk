############################################################################
#
# libre-openipc
#
###########################################################################

LIBRE_VERSION = 1.1.0
LIBRE_SOURCE = v$(LIBRE_VERSION).tar.gz
LIBRE_SITE = https://github.com/baresip/re/archive
LIBRE_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_OPENSSL),y)
LIBRE_DEPENDENCIES += openssl
endif
ifeq ($(BR2_PACKAGE_ZLIB),y)
LIBRE_DEPENDENCIES += zlib
endif

define LIBRE_BUILD_CMDS
    $(TARGET_MAKE_ENV) \
	$(MAKE) -C $(@D) \
	    LIBRE_MK=$(STAGING_DIR)/usr/share/re/re.mk \
	    LIBRE_INC=$(STAGING_DIR)/usr/include/re \
	    LIBRE_SO=$(STAGING_DIR)/usr/lib \
	    CC="$(TARGET_CC)" \
	    EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
	    EXTRA_LFLAGS="-lm" \
	    DESTDIR="$(STAGING_DIR)" \
	    SYSROOT="$(STAGING_DIR)/usr" \
	    SYSROOT_ALT="$(STAGING_DIR)/usr" \
	    RELEASE=1 \
	    CROSS_COMPILE="$(TARGET_CROSS)" \
	    OS=linux \
	    all install
endef


define LIBRE_INSTALL_TARGET_CMDS
        $(INSTALL) -m 644 -D $(@D)/libre.so $(TARGET_DIR)/usr/lib/libre.so
endef

define LIBRE_UNINSTALL_STAGING_CMDS
        $(RM) -r $(STAGING_DIR)/usr/include/re
        $(RM) $(STAGING_DIR)/usr/lib/libre.a
        $(RM) $(STAGING_DIR)/usr/lib/libre.so
endef

define LIBRE_UNINSTALL_TARGET_CMDS
        $(RM) $(TARGET_DIR)/usr/lib/libre.so
endef

$(eval $(generic-package))
$(eval $(host-generic-package))