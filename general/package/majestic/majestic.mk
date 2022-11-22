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

RELEASE := $(shell grep "BR2_DEFCONFIG" $(BR2_CONFIG) | head -1 | cut -d "_" -f 3)

ifeq ($(RELEASE),ultimate)
	# we don't have Majestic binary Ultimate distributions for these
	# platforms so use Lite
	ifeq ($(FAMILY),hi3516av100)
		RELEASE := lite
	else ifeq ($(FAMILY),hi3519v101)
		RELEASE := lite
	endif
endif

MAJESTIC_SOURCE := majestic.$(FAMILY).$(RELEASE).master.tar.bz2

MAJESTIC_DEPENDENCIES = \
	libevent-openipc \
	mbedtls-openipc \
	json-c-openipc \
	zlib \
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
