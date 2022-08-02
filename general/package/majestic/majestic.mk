################################################################################
#
# majestic
#
################################################################################

MAJESTIC_VERSION = current
MAJESTIC_SOURCE =
MAJESTIC_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_LICENSE = MIT
MAJESTIC_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_AMBARELLA_OSDRV_S3L),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.s3l.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.s3l.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_ANYKA_OSDRV_AK3918EV200),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.ak3918ev200.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.ak3918ev200.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_ANYKA_OSDRV_AK3918EV300),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.ak3918ev300.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.ak3918ev300.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_FULLHAN_OSDRV_FH8852V100),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.fh8852v100.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.fh8852v100.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_FULLHAN_OSDRV_FH8852V200),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.fh8852v200.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.fh8852v200.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_GOKE_OSDRV_GK710X),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.gk710x.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.gk710x.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_GRAINMEDIA_OSDRV_GM8136),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.gm8136.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.gm8136.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516AV100),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.hi3516av100.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.hi3516av100.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3519V101),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.hi3519v101.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.hi3519v101.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV100),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.hi3516cv100.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.hi3516cv100.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV200),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.hi3516cv200.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.hi3516cv200.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV300),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.hi3516cv300.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.hi3516cv300.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV500),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.hi3516cv500.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.hi3516cv500.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516EV300),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.hi3516ev300.ultimate.master.tar.bz2
else ifeq ($(BR2_PACKAGE_MAJESTIC_FPV),y)
MAJESTIC_SOURCE = majestic.hi3516ev300.fpv.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.hi3516ev300.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_GOKE_OSDRV_GK7205V200),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.gk7205v200.ultimate.master.tar.bz2
else ifeq ($(BR2_PACKAGE_MAJESTIC_FPV),y)
MAJESTIC_SOURCE = majestic.gk7205v200.fpv.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.gk7205v200.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T20),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.t21.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.t21.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T21),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.t21.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.t21.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T30),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.t30.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.t30.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T31),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.t31.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.t31.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_NOVATEK_OSDRV_NT9856X),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.nt9856x.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.nt9856x.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_ROCKCHIP_OSDRV_RV11xx),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.rv11xx.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.rv11xx.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_MSC313E),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.msc313e.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.msc313e.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_SSC335),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.ssc335.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.ssc335.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM510),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.xm510.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.xm510.lite.master.tar.bz2
endif
endif

ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM530),y)
ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_SOURCE = majestic.xm530.ultimate.master.tar.bz2
else
MAJESTIC_SOURCE = majestic.xm530.lite.master.tar.bz2
endif
endif

define MAJESTIC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
