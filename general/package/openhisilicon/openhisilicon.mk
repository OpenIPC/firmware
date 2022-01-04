################################################################################
#
# openhisilicon
#
################################################################################

OPENHISILICON_VERSION = HEAD
OPENHISILICON_SITE = $(call github,openipc,openhisilicon,$(OPENHISILICON_VERSION))
OPENHISILICON_LICENSE = GPL-3.0
OPENHISILICON_LICENSE_FILES = LICENSE

OPENHISILICON_MODULE_MAKE_OPTS = \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)


$(eval $(kernel-module))
$(eval $(generic-package))
