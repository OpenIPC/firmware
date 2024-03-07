################################################################################
#
# ssv6x5x-openipc
#
################################################################################

SSV6X5X_OPENIPC_SITE = $(call github,openipc,ssv6x5x,$(SSV6X5X_OPENIPC_VERSION))
SSV6X5X_OPENIPC_VERSION = HEAD

SSV6X5X_OPENIPC_LICENSE = GPL-2.0
SSV6X5X_OPENIPC_LICENSE_FILES = COPYING

SSV6X5X_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
