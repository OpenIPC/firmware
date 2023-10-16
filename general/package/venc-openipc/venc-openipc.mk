################################################################################
#
# venc-openipc
#
################################################################################

VENC_OPENIPC_SITE = https://github.com/OpenIPC/silicon_research/releases/download/latest/venc

VENC_OPENIPC_LICENSE = MIT

define VENC_OPENIPC_INSTALL_TARGET_CMDS
	curl -k -L -o $(@D)/venc $(VENC_OPENIPC_SITE)
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/venc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(VENC_OPENIPC_PKGDIR)/files/S98venc $(TARGET_DIR)/etc/init.d/S98venc
endef

$(eval $(generic-package))
