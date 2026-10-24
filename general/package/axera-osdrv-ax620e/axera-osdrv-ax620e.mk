################################################################################
#
# axera-osdrv-ax620e
#
################################################################################

AXERA_OSDRV_AX620E_VERSION =
AXERA_OSDRV_AX620E_SITE =
AXERA_OSDRV_AX620E_LICENSE = GPL-2.0 (open drivers), proprietary (ax_proton/ax_npu)

define AXERA_OSDRV_AX620E_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/5.15.73/axera
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.15.73/axera $(AXERA_OSDRV_AX620E_PKGDIR)/files/kmod/*

	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(AXERA_OSDRV_AX620E_PKGDIR)/files/script/load_axera
endef

$(eval $(generic-package))
