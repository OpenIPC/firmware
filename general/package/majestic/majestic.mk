################################################################################
#
# majestic
#
################################################################################

MAJESTIC_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_SOURCE = majestic.$(MAJESTIC_FAMILY).$(MAJESTIC_VARIANT).master.tar.bz2
MAJESTIC_LICENSE = PROPRIETARY
MAJESTIC_LICENSE_FILES = LICENSE

MAJESTIC_FAMILY = $(OPENIPC_SOC_FAMILY)
MAJESTIC_VARIANT = $(OPENIPC_MAJESTIC)

MAJESTIC_DEPENDENCIES += \
	libevent-openipc \
	libogg-openipc \
	mbedtls-openipc \
	opus-openipc \
	json-c

MAJESTIC_FILTER = hi3516ev200 gk7205v200 infinity6b0 infinity6e t31
ifneq ($(filter $(MAJESTIC_FILTER),$(MAJESTIC_FAMILY)),)
	MAJESTIC_DEPENDENCIES += majestic-plugins
endif

define MAJESTIC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/majestic.yaml

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(MAJESTIC_PKGDIR)/files/S95majestic

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
