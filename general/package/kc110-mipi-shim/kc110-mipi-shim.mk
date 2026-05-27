################################################################################
#
# kc110-mipi-shim
#
################################################################################

KC110_MIPI_SHIM_SITE_METHOD = local
KC110_MIPI_SHIM_SITE = $(KC110_MIPI_SHIM_PKGDIR)/src

define KC110_MIPI_SHIM_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" -C $(@D)
endef

define KC110_MIPI_SHIM_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libfixmipi.so $(TARGET_DIR)/usr/lib/libfixmipi.so
endef

$(eval $(generic-package))
