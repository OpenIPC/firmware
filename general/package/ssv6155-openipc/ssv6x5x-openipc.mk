################################################################################
#
# ssv6155-openipc
#
################################################################################

SSV6155_OPENIPC_SITE = $(call github,openipc,ssv6x5x,$(SSV6155_OPENIPC_VERSION))
SSV6155_OPENIPC_VERSION = ssv6155

SSV6155_OPENIPC_LICENSE = GPL-2.0
SSV6155_OPENIPC_LICENSE_FILES = COPYING

SSV6155_OPENIPC_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
