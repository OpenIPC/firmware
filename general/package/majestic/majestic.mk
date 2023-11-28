################################################################################
#
# majestic
#
################################################################################

MAJESTIC_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_SOURCE = majestic.$(MAJESTIC_FAMILY).$(MAJESTIC_RELEASE).master.tar.bz2

MAJESTIC_LICENSE = PROPRIETARY
MAJESTIC_LICENSE_FILES = LICENSE

MAJESTIC_FAMILY = $(OPENIPC_SOC_FAMILY)
MAJESTIC_RELEASE = $(OPENIPC_FLAVOR)

# we don't have Majestic ultimate for these platforms
MAJESTIC_LIST = hi3516av100 hi3519v101

ifneq ($(filter $(MAJESTIC_LIST),$(MAJESTIC_FAMILY)),)
	MAJESTIC_RELEASE = lite
endif

ifeq ($(MAJESTIC_RELEASE),lte)
	MAJESTIC_RELEASE = fpv
endif

MAJESTIC_DEPENDENCIES = \
	libevent-openipc \
	libogg-openipc \
	mbedtls-openipc \
	opus-openipc \
	json-c \
	zlib

MAJESTIC_STRIP_COMPONENTS = 0

define MAJESTIC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(MAJESTIC_PKGDIR)/files/S95majestic

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic

	rm -rf $(MAJESTIC_DL_DIR)
endef

$(eval $(generic-package))
