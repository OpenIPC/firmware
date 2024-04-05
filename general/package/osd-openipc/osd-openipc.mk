################################################################################
#
# osd-openipc
#
################################################################################

OSD_OPENIPC_SITE = $(call github,openipc,osd,$(OSD_OPENIPC_VERSION))
OSD_OPENIPC_VERSION = HEAD

OSD_OPENIPC_TARGET = osd_server

ifeq ($(OPENIPC_SOC_VENDOR),goke)
	OSD_OPENIPC_VENDOR = osd-goke
	OSD_OPENIPC_OSDRV = $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/lib
else ifeq ($(OPENIPC_SOC_VENDOR),hisilicon)
	OSD_OPENIPC_VENDOR = osd-hisi
	OSD_OPENIPC_OSDRV = $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib
else
	OSD_OPENIPC_VENDOR = osd-star
	OSD_OPENIPC_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)/files/lib
endif

define OSD_OPENIPC_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) DRV=$(OSD_OPENIPC_OSDRV) TARGET=$(OSD_OPENIPC_TARGET) $(OSD_OPENIPC_VENDOR) -C $(@D)/osd
endef

define OSD_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/osd/$(OSD_OPENIPC_TARGET)
endef

$(eval $(generic-package))
