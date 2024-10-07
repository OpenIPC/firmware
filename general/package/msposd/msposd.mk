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
else
	MSPOSD_FAMILY = star6e
	MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)
endif

define MSPOSD_BUILD_CMDS
	$(INSTALL) -m 644 -t $(@D) $(MSPOSD_PKGDIR)/files/Makefile
	$(MAKE) CC=$(TARGET_CC) DRV=$(MSPOSD_OSDRV)/files/lib $(MSPOSD_FAMILY) -C $(@D)
endef

define MSPOSD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/msposd
endef

$(eval $(generic-package))
