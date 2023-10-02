################################################################################
#
# venc-openipc
#
################################################################################

VENC_OPENIPC_LICENSE = MIT

define VENC_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(VENC_OPENIPC_PKGDIR)/files/venc $(TARGET_DIR)/usr/bin/venc
endef

$(eval $(generic-package))
