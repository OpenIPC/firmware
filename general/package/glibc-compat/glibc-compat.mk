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

# Symlink names a vendor glibc binary asks for: ld-linux.so.3 for soft-float
# vendors (Hi3520DV200), ld-linux-armhf.so.3 for hard-float ones (SigmaStar
# infinity3). musl's libc.so is both the dynamic linker and the C library, so
# pointing them at it is enough for the loader to satisfy NEEDED entries.
# libglibc-compat.so, with the symbols musl lacks, is installed to staging as
# well as the target: the executable that loads the vendor libs links it ahead
# of libc, which is what makes its __xstat win over musl's (see Config.in).
define GLIBC_COMPAT_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(GLIBC_COMPAT_PKGDIR)/src/libglibc-compat-static.a \
		$(STAGING_DIR)/usr/lib/libglibc-compat-static.a
	$(INSTALL) -D -m 0755 $(GLIBC_COMPAT_PKGDIR)/src/libglibc-compat.so \
		$(STAGING_DIR)/usr/lib/libglibc-compat.so
endef

define GLIBC_COMPAT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(GLIBC_COMPAT_PKGDIR)/src/libglibc-compat.so \
		$(TARGET_DIR)/usr/lib/libglibc-compat.so
	ln -sf libc.so $(TARGET_DIR)/lib/libc.so.6
	ln -sf libc.so $(TARGET_DIR)/lib/libm.so.6
	ln -sf libc.so $(TARGET_DIR)/lib/ld-linux.so.3
	ln -sf libc.so $(TARGET_DIR)/lib/ld-linux-armhf.so.3
endef

$(eval $(generic-package))
