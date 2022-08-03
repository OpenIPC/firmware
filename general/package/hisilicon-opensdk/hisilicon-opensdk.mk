################################################################################
#
# openhisilicon
#
################################################################################

HISILICON_OPENSDK_VERSION = 6b1e33444bd044252fed702b0eee8e5375ea8c6e
HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

ifndef BOARD
	@$$(call MESSAGE,"Variable BOARD must be defined")
	@exit
else
	CHIP := $(shell echo $(BOARD) | cut -d "_" -f 3)
endif

HISILICON_OPENSDK_MODULE_SUBDIRS = kernel
HISILICON_OPENSDK_MODULE_MAKE_OPTS = \
	CHIPSET=$(CHIP)

$(eval $(kernel-module))
$(eval $(generic-package))
