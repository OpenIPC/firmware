################################################################################
#
# openhisilicon
#
################################################################################

HISILICON_OPENSDK_VERSION = 6b1e33444bd044252fed702b0eee8e5375ea8c6e
HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

FAMILY = $(shell grep "/board/" $(BR2_CONFIG) | head -1 | cut -d "/" -f 3)

HISILICON_OPENSDK_MODULE_SUBDIRS = kernel
HISILICON_OPENSDK_MODULE_MAKE_OPTS = \
	CHIPSET=$(FAMILY)

$(eval $(kernel-module))
$(eval $(generic-package))
