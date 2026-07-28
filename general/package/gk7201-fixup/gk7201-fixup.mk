################################################################################
#
# gk7201-fixup
#
################################################################################

GK7201_FIXUP_VERSION = 1.0
GK7201_FIXUP_SITE = $(GK7201_FIXUP_PKGDIR)/src
GK7201_FIXUP_SITE_METHOD = local
GK7201_FIXUP_LICENSE = GPL-2.0
GK7201_FIXUP_LICENSE_FILES =

# gk7201_fixup.ko is built by Buildroot's kernel-module infra from src/Makefile
# (obj-m). libgk_shim.so is a plain shared object, built here.
define GK7201_FIXUP_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) -shared -fPIC -Wall \
		-o $(@D)/libgk_shim.so $(@D)/gk_shim.c -ldl
endef

define GK7201_FIXUP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 644 $(@D)/libgk_shim.so \
		$(TARGET_DIR)/usr/lib/libgk_shim.so
	$(INSTALL) -D -m 644 $(GK7201_FIXUP_PKGDIR)/files/majestic-env \
		$(TARGET_DIR)/etc/default/majestic
endef

$(eval $(kernel-module))
$(eval $(generic-package))
