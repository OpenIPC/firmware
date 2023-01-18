################################################################################
#
# AltoBeam atbm603x wifi driver
#
################################################################################

ATBM603X_VERSION = f01238ec655f8736d54033b986fbf889f1eef01c
ATBM603X_SITE = $(call github,OpenIPC,atbm_60xx,$(ATBM603X_VERSION))
ATBM603X_LICENSE = GPL-2.0

ATBM603X_MODULE_MAKE_OPTS = \
	CONFIG_ATBM601x=n \
	CONFIG_ATBM602x=n \
	CONFIG_ATBM603x=y \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
