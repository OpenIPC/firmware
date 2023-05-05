################################################################################
#
# xiongmai_opensdk_xm510
#
################################################################################

XIONGMAI_OPENSDK_XM510_LICENSE = MIT
XIONGMAI_OPENSDK_XM510_LICENSE_FILES = LICENSE

define XIONGMAI_OPENSDK_XM510_EXTRACT_CMDS
	cp $(XIONGMAI_OPENSDK_XM510_PKGDIR)/src/lib/wrapper.c $(@D)/
endef

define XIONGMAI_OPENSDK_XM510_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -fPIC -shared -s wrapper.c -o libwrapper.so )
endef

define XIONGMAI_OPENSDK_XM510_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/libwrapper.so $(TARGET_DIR)/lib/libwrapper.so
	ln -sfv libwrapper.so $(TARGET_DIR)/lib/libwrapper.so.0
endef

$(eval $(generic-package))
