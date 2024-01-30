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

MAJESTIC_ULTIMATE = hi3516av100 hi3519v101 t20 t21 t40
ifneq ($(filter $(MAJESTIC_ULTIMATE),$(MAJESTIC_FAMILY)),)
	MAJESTIC_RELEASE = lite
endif

ifeq ($(MAJESTIC_RELEASE),lte)
	MAJESTIC_RELEASE = fpv
endif

MAJESTIC_VENDOR = hisilicon goke ingenic sigmastar
ifneq ($(filter $(MAJESTIC_VENDOR),$(OPENIPC_SOC_VENDOR)),)
	MAJESTIC_DEPENDENCIES += majestic-plugins
endif

MAJESTIC_DEPENDENCIES += \
	libevent-openipc \
	libogg-openipc \
	mbedtls-openipc \
	opus-openipc \
	json-c

define MAJESTIC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/majestic.yaml
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(MAJESTIC_PKGDIR)/files/S95majestic

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
