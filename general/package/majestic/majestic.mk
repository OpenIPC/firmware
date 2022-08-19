################################################################################
#
# majestic
#
################################################################################

MAJESTIC_VERSION = current
MAJESTIC_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_LICENSE = PROPRIETARY
MAJESTIC_LICENSE_FILES = LICENSE

FAMILY := $(shell grep "/board/" $(BR2_CONFIG) | head -1 | cut -d "/" -f 3)

# TODO: change field number removing unknown_unknown
RELEASE := $(shell grep "BR2_DEFCONFIG" $(BR2_CONFIG) | head -1 | cut -d "_" -f 5)

ifneq ($(RELEASE),ultimate)
	RELEASE := lite
endif

ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T20),y)
	FAMILY := t21
else ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T30),y)
	FAMILY := t31
else ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T31),y)
	FAMILY := t31
else ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_SSC335),y)
	FAMILY := ssc335
endif

MAJESTIC_SOURCE := majestic.$(FAMILY).$(RELEASE).master.tar.bz2

MAJESTIC_DEPENDENCIES = \
	libevent-openipc \
	mbedtls-openipc \
	json-c-openipc \
	zlib \
	lame-openipc \
	opus-openipc \
	libogg-openipc

define MAJESTIC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
