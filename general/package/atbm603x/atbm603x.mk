################################################################################
#
# AltoBeam atbm603x wifi driver
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
ATBM603X_SITE_METHOD = git
ATBM603X_SITE = https://github.com/openipc/atbm_60xx
ATBM603X_VERSION = $(shell git ls-remote $(ATBM603X_SITE) HEAD | head -1 | cut -f1)
else
ATBM603X_SITE = https://github.com/openipc/atbm_60xx/archive
ATBM603X_SOURCE = master.tar.gz
endif

ATBM603X_LICENSE = GPL-2.0

ATBM603X_MODULE_MAKE_OPTS = \
	CONFIG_ATBM601x=n \
	CONFIG_ATBM602x=n \
	CONFIG_ATBM603x=y \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
