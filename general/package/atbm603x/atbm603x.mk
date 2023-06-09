################################################################################
#
# AltoBeam atbm603x wifi driver
#
################################################################################

ATBM603X_VERSION = HEAD
ATBM603X_SITE = $(call github,openipc,atbm_60xx,$(ATBM603X_VERSION))
ATBM603X_LICENSE = GPL-2.0

ATBM603X_MODULE_MAKE_OPTS = \
	CONFIG_ATBM601x=n \
	CONFIG_ATBM602x=n \
	CONFIG_ATBM603x=y \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
