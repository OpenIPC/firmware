################################################################################
#
# atbm603x-comb-openipc
#
################################################################################

# OpenIPC/atbm_60xx's atbm603x_comb/ directory: AltoBeam's apollo driver
# (DRIVER_VER 2874) with the AsmLite and Ares-B firmware, as Khadas ships it.
ATBM603X_COMB_OPENIPC_SITE = $(call github,openipc,atbm_60xx,$(ATBM603X_COMB_OPENIPC_VERSION))
ATBM603X_COMB_OPENIPC_VERSION = 543b02dc8871a1bc4df90cd56e07305b95c3ec9c

ATBM603X_COMB_OPENIPC_LICENSE = GPL-2.0

# The directory carries its own .config (USB, firmware built in, module
# atbm603x_comb_wifi_usb). ATBM_BUILD_IN_KERNEL=n makes its Makefiles
# read it from WIFI_PROJ_DIR.
ATBM603X_COMB_OPENIPC_MODULE_SUBDIRS = atbm603x_comb
ATBM603X_COMB_OPENIPC_MODULE_MAKE_OPTS = \
	ATBM_BUILD_IN_KERNEL=n \
	WIFI_PROJ_DIR=$(@D)/atbm603x_comb

$(eval $(kernel-module))
$(eval $(generic-package))
