################################################################################
#
# venc-openipc
#
################################################################################

VENC_OPENIPC_SITE = $(call github,openipc,silicon_research,$(VENC_OPENIPC_VERSION))
VENC_OPENIPC_VERSION = HEAD

VENC_OPENIPC_LICENSE = MIT

ifeq ($(OPENIPC_SOC_FAMILY),infinity6c)
	VENC_OPENIPC_OUTPUT = star
	VENC_OPENIPC_TARGET = star-c
	VENC_OPENIPC_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)/files/lib
else ifeq ($(OPENIPC_SOC_FAMILY),infinity6e)
	VENC_OPENIPC_OUTPUT = star
	VENC_OPENIPC_TARGET = star-e
	VENC_OPENIPC_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)/files/lib
else ifeq ($(OPENIPC_SOC_FAMILY),hi3516ev200)
	VENC_OPENIPC_OUTPUT = venc
	VENC_OPENIPC_TARGET = venc-hisi
	VENC_OPENIPC_OSDRV = $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib
else
	VENC_OPENIPC_OUTPUT = venc
	VENC_OPENIPC_TARGET = venc-goke
	VENC_OPENIPC_OSDRV = $(GOKE_OSDRV_GK7205V200_PKGDIR)/files/lib
endif

define VENC_OPENIPC_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) DRV=$(VENC_OPENIPC_OSDRV) $(VENC_OPENIPC_TARGET) -C $(@D)/$(VENC_OPENIPC_OUTPUT)
endef

define VENC_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(VENC_OPENIPC_PKGDIR)/files/S98venc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(VENC_OPENIPC_PKGDIR)/files/venc.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/$(VENC_OPENIPC_OUTPUT)/$(VENC_OPENIPC_OUTPUT)
endef

$(eval $(generic-package))
