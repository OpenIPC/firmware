################################################################################
#
# ssv615x-openipc
#
################################################################################

SSV615X_OPENIPC_SITE = $(call github,openipc,ssv6x5x,$(SSV615X_OPENIPC_VERSION))
SSV615X_OPENIPC_VERSION = ssv615x

SSV615X_OPENIPC_LICENSE = GPL-2.0
SSV615X_OPENIPC_LICENSE_FILES = COPYING

SSV615X_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
