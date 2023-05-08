################################################################################
#
# majestic
#
################################################################################

MAJESTIC_VERSION = current
MAJESTIC_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_LICENSE = PROPRIETARY
MAJESTIC_LICENSE_FILES = LICENSE

MAJESTIC_FAMILY = $(EXTERNAL_FAMILY)
MAJESTIC_RELEASE = $(EXTERNAL_RELEASE)

ifeq ($(MAJESTIC_RELEASE),ultimate)
	# we don't have Majestic binary Ultimate distributions for these
	# platforms so use Lite
	ifeq ($(MAJESTIC_FAMILY),hi3516av100)
		MAJESTIC_RELEASE = lite
	else ifeq ($(MAJESTIC_FAMILY),hi3519v101)
		MAJESTIC_RELEASE = lite
	endif
endif

ifeq ($(MAJESTIC_RELEASE),lte)
	MAJESTIC_RELEASE = fpv
endif

MAJESTIC_SOURCE = majestic.$(MAJESTIC_FAMILY).$(MAJESTIC_RELEASE).master.tar.bz2

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

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(MAJESTIC_PKGDIR)/files/S95majestic
endef

$(eval $(generic-package))
