################################################################################
#
# venc-openipc
#
################################################################################

VENC_OPENIPC_SITE = $(call github,openipc,silicon_research,$(VENC_OPENIPC_VERSION))
VENC_OPENIPC_VERSION = master

VENC_OPENIPC_LICENSE = MIT

ifeq ($(OPENIPC_SOC_VENDOR),hisilicon)
	VENC_OPENIPC_TARGET = venc-hisi
	VENC_OPENIPC_OSDRV = $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib
else
	VENC_OPENIPC_TARGET = venc-goke
	VENC_OPENIPC_OSDRV = $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/lib
endif

define VENC_OPENIPC_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) DRV=$(VENC_OPENIPC_OSDRV) $(VENC_OPENIPC_TARGET) -C $(@D)/venc
endef

define VENC_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(VENC_OPENIPC_PKGDIR)/files/S98venc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(VENC_OPENIPC_PKGDIR)/files/venc.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/venc/venc
endef

$(eval $(generic-package))
