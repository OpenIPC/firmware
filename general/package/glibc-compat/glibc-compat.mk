################################################################################
#
# glibc-compat
#
################################################################################

GLIBC_COMPAT_VERSION =
GLIBC_COMPAT_SITE =
GLIBC_COMPAT_LICENSE = MIT
GLIBC_COMPAT_INSTALL_STAGING = YES

define GLIBC_COMPAT_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(GLIBC_COMPAT_PKGDIR)/src all
endef

# Symlink names a vendor glibc binary still asks for once libglibc-compat.so
# is loaded. musl's libc.so is both the dynamic linker and the C library, so
# pointing all four names at it is enough for the loader to satisfy NEEDED
# entries; the libglibc-compat.so providing missing symbols is pulled in by
# the LD_PRELOAD-equivalent NEEDED added to vendor wrapper scripts.
define GLIBC_COMPAT_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(GLIBC_COMPAT_PKGDIR)/src/libglibc-compat-static.a \
		$(STAGING_DIR)/usr/lib/libglibc-compat-static.a
endef

define GLIBC_COMPAT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(GLIBC_COMPAT_PKGDIR)/src/libglibc-compat.so \
		$(TARGET_DIR)/usr/lib/libglibc-compat.so
	ln -sf libc.so $(TARGET_DIR)/lib/libc.so.6
	ln -sf libc.so $(TARGET_DIR)/lib/libm.so.6
	ln -sf libc.so $(TARGET_DIR)/lib/ld-linux.so.3
endef

$(eval $(generic-package))
