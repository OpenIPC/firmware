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

FAMILY := $(shell grep "/board/" $(BR2_CONFIG) | head -1 | cut -d "/" -f 3)

ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
BUILD = ultimate
else ifeq ($(BR2_PACKAGE_MAJESTIC_FPV),y)
BUILD = fpv
else
BUILD = lite
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T20),y)
MAJESTIC_SOURCE = majestic.t21.$(BUILD).master.tar.bz2
else ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T21),y)
MAJESTIC_SOURCE = majestic.t21.$(BUILD).master.tar.bz2
else ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T30),y)
MAJESTIC_SOURCE = majestic.t30.$(BUILD).master.tar.bz2
else ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T31),y)
MAJESTIC_SOURCE = majestic.t31.$(BUILD).master.tar.bz2
else ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_MSC313E),y)
MAJESTIC_SOURCE = majestic.msc313e.$(BUILD).master.tar.bz2
else ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_SSC335),y)
MAJESTIC_SOURCE = majestic.ssc335.$(BUILD).master.tar.bz2
else
MAJESTIC_SOURCE = majestic.$(FAMILY).$(BUILD).master.tar.bz2
endif

define MAJESTIC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
