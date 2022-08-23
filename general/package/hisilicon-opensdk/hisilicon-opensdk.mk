################################################################################
#
# openhisilicon
#
################################################################################

HISILICON_OPENSDK_VERSION = 96dc0a811d3b6f119b7ad6eca57ddaa8e20d8dd9
HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

FAMILY = $(shell grep "/board/" $(BR2_CONFIG) | head -1 | cut -d "/" -f 3)

HISILICON_OPENSDK_MODULE_SUBDIRS = kernel
HISILICON_OPENSDK_MODULE_MAKE_OPTS = \
	CHIPSET=$(FAMILY)

$(eval $(kernel-module))
$(eval $(generic-package))
