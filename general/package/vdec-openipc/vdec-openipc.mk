################################################################################
#
# vdec-openipc
#
################################################################################

VDEC_OPENIPC_LICENSE = MIT

define VDEC_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(VDEC_OPENIPC_PKGDIR)/files/vdec $(TARGET_DIR)/usr/bin/vdec
endef

$(eval $(generic-package))
