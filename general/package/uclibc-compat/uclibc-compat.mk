################################################################################
#
# uclibc-compat
#
################################################################################

UCLIBC_COMPAT_VERSION =
UCLIBC_COMPAT_SITE =
UCLIBC_COMPAT_LICENSE = MIT
UCLIBC_COMPAT_INSTALL_STAGING = YES

define UCLIBC_COMPAT_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(UCLIBC_COMPAT_PKGDIR)/src all
endef

define UCLIBC_COMPAT_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0755 $(UCLIBC_COMPAT_PKGDIR)/src/libuclibc-compat.so $(STAGING_DIR)/usr/lib/libuclibc-compat.so
	$(INSTALL) -D -m 0644 $(UCLIBC_COMPAT_PKGDIR)/src/libuclibc-compat-static.a $(STAGING_DIR)/usr/lib/libuclibc-compat-static.a
endef

define UCLIBC_COMPAT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(UCLIBC_COMPAT_PKGDIR)/src/libuclibc-compat.so $(TARGET_DIR)/usr/lib/libuclibc-compat.so
endef

$(eval $(generic-package))
