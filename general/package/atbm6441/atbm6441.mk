################################################################################
#
# atbm6441
#
################################################################################

ATBM6441_SITE = $(call github,gtxaspec,atbm6441,$(ATBM6441_VERSION))
ATBM6441_VERSION = master

ATBM6441_LICENSE = GPL-2.0
ATBM6441_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
