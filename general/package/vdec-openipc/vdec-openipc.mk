################################################################################
#
# vdec-openipc
#
################################################################################

VDEC_OPENIPC_SITE = https://github.com/OpenIPC/silicon_research/releases/download/latest/vdec

VDEC_OPENIPC_LICENSE = MIT

define VDEC_OPENIPC_INSTALL_TARGET_CMDS
	curl -k -L -o $(@D)/vdec $(VDEC_OPENIPC_SITE)
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/vdec

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(VDEC_OPENIPC_PKGDIR)/files/S98vdec $(TARGET_DIR)/etc/init.d/S98vdec
endef

$(eval $(generic-package))
