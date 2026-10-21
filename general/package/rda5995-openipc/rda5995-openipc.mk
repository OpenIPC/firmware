################################################################################
#
# rda5995-openipc
#
################################################################################

RDA5995_OPENIPC_VERSION = 06fd4b61252788bc46a66c88fd2773493ab5dd2a
RDA5995_OPENIPC_SITE = $(call github,openipc,rda-wlan,$(RDA5995_OPENIPC_VERSION))

# Rdamicro's permission notice heads every source file; the module
# declares GPL v2.
RDA5995_OPENIPC_LICENSE = ISC, GPL-2.0
RDA5995_OPENIPC_LICENSE_FILES = rdawlan/wland_linux.c

RDA5995_OPENIPC_MODULE_SUBDIRS = rdawlan
RDA5995_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RDAWFMAC_USB=y \
	CONFIG_RDAWFMAC_SDIO=

$(eval $(kernel-module))
$(eval $(generic-package))
