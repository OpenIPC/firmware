################################################################################
#
# altobeam ATBM6441 WiFi driver
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
ATBM6441_SITE_METHOD = git
ATBM6441_SITE = https://github.com/gtxaspec/atbm6441
ATBM6441_VERSION = $(shell git ls-remote $(ATBM6441_SITE) HEAD | head -1 | cut -f1)
else
ATBM6441_SITE = https://github.com/gtxaspec/atbm6441/archive
ATBM6441_SOURCE = master.tar.gz
endif

ATBM6441_LICENSE = GPL-2.0
ATBM6441_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
