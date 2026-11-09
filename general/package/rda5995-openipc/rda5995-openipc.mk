################################################################################
#
# rda5995-openipc
#
################################################################################

RDA5995_OPENIPC_VERSION = 71188e50a9aee05e5781ccb80cb418412237d7a8
RDA5995_OPENIPC_SITE = $(call github,openipc,rda-wlan,$(RDA5995_OPENIPC_VERSION))

# Rdamicro's permission notice heads every source file; the module
# declares GPL v2.
RDA5995_OPENIPC_LICENSE = ISC, GPL-2.0
RDA5995_OPENIPC_LICENSE_FILES = rdawlan/wland_linux.c

ifeq ($(BR2_PACKAGE_RDA5995_OPENIPC_RDAWLAN_RDA5995),y)
RDA5995_OPENIPC_LICENSE_FILES = rdawlan-rda5995/wland_linux.c
RDA5995_OPENIPC_MODULE_SUBDIRS = rdawlan-rda5995

# The station firmware only: the AP images would cost 160 KB of rootfs
define RDA5995_OPENIPC_INSTALL_FIRMWARE
	$(INSTALL) -m 0644 -D -t $(TARGET_DIR)/lib/firmware \
		$(@D)/rdawlan-rda5995/firmware/*_p2p.bin
endef
RDA5995_OPENIPC_POST_INSTALL_TARGET_HOOKS += RDA5995_OPENIPC_INSTALL_FIRMWARE
else
RDA5995_OPENIPC_MODULE_SUBDIRS = rdawlan
RDA5995_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RDAWFMAC_USB=y \
	CONFIG_RDAWFMAC_SDIO=
endif

$(eval $(kernel-module))
$(eval $(generic-package))
