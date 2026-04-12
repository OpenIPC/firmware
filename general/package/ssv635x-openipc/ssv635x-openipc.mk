################################################################################
#
# ssv635x-openipc
#
################################################################################

SSV635X_OPENIPC_SITE = $(call github,openipc,ssv6x5x,$(SSV635X_OPENIPC_VERSION))
SSV635X_OPENIPC_VERSION = HEAD

SSV635X_OPENIPC_LICENSE = GPL-2.0
SSV635X_OPENIPC_LICENSE_FILES = COPYING

SSV635X_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
