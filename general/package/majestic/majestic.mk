################################################################################
#
# majestic
#
################################################################################

MAJESTIC_VERSION = current
MAJESTIC_SOURCE = majestic.$(MAJESTIC_FAMILY).$(MAJESTIC_RELEASE).master.tar.bz2
MAJESTIC_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_LICENSE = PROPRIETARY
MAJESTIC_LICENSE_FILES = LICENSE

MAJESTIC_FAMILY = $(OPENIPC_SOC_FAMILY)
MAJESTIC_RELEASE = $(OPENIPC_FLAVOR)

ifeq ($(MAJESTIC_RELEASE),ultimate)
	# we don't have Majestic ultimate for these platforms
	ifeq ($(MAJESTIC_FAMILY),hi3516av100)
		MAJESTIC_RELEASE = lite
	else ifeq ($(MAJESTIC_FAMILY),hi3519v101)
		MAJESTIC_RELEASE = lite
	endif
endif

ifeq ($(MAJESTIC_RELEASE),lte)
	MAJESTIC_RELEASE = fpv
endif

MAJESTIC_DEPENDENCIES = \
	json-c-openipc \
	libevent-openipc \
	libogg-openipc \
	mbedtls-openipc \
	opus-openipc \
	zlib

define MAJESTIC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(MAJESTIC_PKGDIR)/files/S95majestic

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
