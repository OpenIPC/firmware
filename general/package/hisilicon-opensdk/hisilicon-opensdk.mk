################################################################################
#
# openhisilicon
#
################################################################################

HISILICON_OPENSDK_VERSION = fc315a59157e20654f2a572d24b1f973edaa6588
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
