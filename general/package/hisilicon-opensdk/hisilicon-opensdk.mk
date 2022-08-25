################################################################################
#
# openhisilicon
#
################################################################################

HISILICON_OPENSDK_VERSION = 3094c2a8933abe5fc9ecc62b388ad2fbdd34724f
HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

FAMILY = $(shell grep "/board/" $(BR2_CONFIG) | head -1 | cut -d "/" -f 3)

HISILICON_OPENSDK_MODULE_SUBDIRS = kernel
HISILICON_OPENSDK_MODULE_MAKE_OPTS = \
	CHIPARCH=$(FAMILY)

$(eval $(kernel-module))
$(eval $(generic-package))
