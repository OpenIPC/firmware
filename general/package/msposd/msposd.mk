################################################################################
#
# msposd
#
################################################################################

MSPOSD_SITE = $(call github,openipc,msposd,$(MSPOSD_VERSION))
MSPOSD_VERSION = 2298981b32dce8cc92b5198703117cd515c30fca

ifeq ($(OPENIPC_SOC_FAMILY),gk7205v200)
	MSPOSD_FAMILY = goke
	MSPOSD_OSDRV = $(GOKE_OSDRV_GK7205V200_PKGDIR)
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516ev200)
	MSPOSD_FAMILY = hisi
	MSPOSD_OSDRV = $(HISILICON_OSDRV_HI3516EV200_PKGDIR)
else ifeq ($(OPENIPC_SOC_FAMILY),infinity6b0)
	MSPOSD_FAMILY = star6b0
	MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)
else
	MSPOSD_FAMILY = star6e
	MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)
endif

define MSPOSD_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) DRV=$(MSPOSD_OSDRV)/files/lib $(MSPOSD_FAMILY) OUTPUT=$(@D) -C $(@D)
endef

define MSPOSD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/msposd
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/safeboot.sh

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/fonts/common/font.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/fonts/common/font_hd.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/vtxmenu.ini
endef

$(eval $(generic-package))
