################################################################################
#
# msposd
#
################################################################################

MSPOSD_SITE = $(call github,openipc,msposd,$(MSPOSD_VERSION))
MSPOSD_VERSION = HEAD

ifeq ($(OPENIPC_SOC_FAMILY),gk7205v200)
	MSPOSD_FAMILY = goke
	MSPOSD_OSDRV = $(GOKE_OSDRV_GK7205V200_PKGDIR)
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516ev200)
	MSPOSD_FAMILY = hisi
	MSPOSD_OSDRV = $(HISILICON_OSDRV_HI3516EV200_PKGDIR)
else ifeq ($(OPENIPC_SOC_FAMILY),infinity6b0)
	MSPOSD_FAMILY = star6b0
	MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)
else ifeq ($(OPENIPC_SOC_FAMILY),infinity6c)
	MSPOSD_FAMILY = star6c
	MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)
else
	MSPOSD_FAMILY = star6e
	MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)
endif

define MSPOSD_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) TOOLCHAIN=$(STAGING_DIR) DRV=$(MSPOSD_OSDRV)files/lib $(MSPOSD_FAMILY) OUTPUT=$(@D)/msposd -C $(@D)
endef

define MSPOSD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/msposd
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/safeboot.sh

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/fonts/font_inav.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/fonts/font_inav_hd.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/fonts/font_btfl.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/fonts/font_btfl_hd.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/fonts/font_ardu.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/fonts/font_ardu_hd.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/vtxmenu.ini
endef

define MSPOSD_INSTALL_LIBRARIES
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(MSPOSD_OSDRV)/files/lib/libcam_os_wrapper.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(MSPOSD_OSDRV)/files/lib/libmi_rgn.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(MSPOSD_OSDRV)/files/lib/libmi_sys.so
endef

ifeq ($(OPENIPC_SOC_VENDOR),sigmastar)
MSPOSD_POST_INSTALL_TARGET_HOOKS += MSPOSD_INSTALL_LIBRARIES
endif

$(eval $(generic-package))
