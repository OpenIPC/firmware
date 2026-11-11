################################################################################
#
# fullhan-osdrv-fh8626
#
################################################################################

FULLHAN_OSDRV_FH8626_VERSION =
FULLHAN_OSDRV_FH8626_SITE =
FULLHAN_OSDRV_FH8626_LICENSE = MIT

# There is no vendor SDK, so nothing goes in files/, which make toolchain
# bundles into the SDK as one
define FULLHAN_OSDRV_FH8626_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(FULLHAN_OSDRV_FH8626_PKGDIR)/load_fullhan \
		$(TARGET_DIR)/usr/bin/load_fullhan
endef

$(eval $(generic-package))
