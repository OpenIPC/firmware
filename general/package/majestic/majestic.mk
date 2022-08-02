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

ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
BUILD = ultimate
else ifeq ($(BR2_PACKAGE_MAJESTIC_FPV),y)
BUILD = fpv
else
BUILD = lite
endif

ifeq ($(BR2_PACKAGE_AMBARELLA_OSDRV_S3L),y)
MAJESTIC_SOURCE = majestic.s3l.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_ANYKA_OSDRV_AK3918EV200),y)
MAJESTIC_SOURCE = majestic.ak3918ev200.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_ANYKA_OSDRV_AK3918EV300),y)
MAJESTIC_SOURCE = majestic.ak3918ev300.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_FULLHAN_OSDRV_FH8852V100),y)
MAJESTIC_SOURCE = majestic.fh8852v100.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_FULLHAN_OSDRV_FH8852V200),y)
MAJESTIC_SOURCE = majestic.fh8852v200.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_GOKE_OSDRV_GK710X),y)
MAJESTIC_SOURCE = majestic.gk710x.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_GOKE_OSDRV_GK7205V200),y)
MAJESTIC_SOURCE = majestic.gk7205v200.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_GRAINMEDIA_OSDRV_GM8136),y)
MAJESTIC_SOURCE = majestic.gm8136.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516AV100),y)
MAJESTIC_SOURCE = majestic.hi3516av100.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3519V101),y)
MAJESTIC_SOURCE = majestic.hi3519v101.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV100),y)
MAJESTIC_SOURCE = majestic.hi3516cv100.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV200),y)
MAJESTIC_SOURCE = majestic.hi3516cv200.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV300),y)
MAJESTIC_SOURCE = majestic.hi3516cv300.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV500),y)
MAJESTIC_SOURCE = majestic.hi3516cv500.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516EV300),y)
MAJESTIC_SOURCE = majestic.hi3516ev300.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T20),y)
MAJESTIC_SOURCE = majestic.t21.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T21),y)
MAJESTIC_SOURCE = majestic.t21.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T30),y)
MAJESTIC_SOURCE = majestic.t30.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T31),y)
MAJESTIC_SOURCE = majestic.t31.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_NOVATEK_OSDRV_NT9856X),y)
MAJESTIC_SOURCE = majestic.nt9856x.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_ROCKCHIP_OSDRV_RV11xx),y)
MAJESTIC_SOURCE = majestic.rv11xx.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_MSC313E),y)
MAJESTIC_SOURCE = majestic.msc313e.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_SSC335),y)
MAJESTIC_SOURCE = majestic.ssc335.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM510),y)
MAJESTIC_SOURCE = majestic.xm510.$(BUILD).master.tar.bz2
endif

ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM530),y)
MAJESTIC_SOURCE = majestic.xm530.$(BUILD).master.tar.bz2
endif

define MAJESTIC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
