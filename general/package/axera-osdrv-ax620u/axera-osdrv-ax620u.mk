################################################################################
#
# axera-osdrv-ax620u
#
################################################################################

AXERA_OSDRV_AX620U_VERSION =
AXERA_OSDRV_AX620U_SITE =
AXERA_OSDRV_AX620U_LICENSE = proprietary

define AXERA_OSDRV_AX620U_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.125/axera
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.125/axera $(AXERA_OSDRV_AX620U_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(AXERA_OSDRV_AX620U_PKGDIR)/files/script/load_axera
endef

$(eval $(generic-package))
