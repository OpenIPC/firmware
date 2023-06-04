################################################################################
#
# ingenic-opensdk
#
################################################################################

INGENIC_OPENSDK_VERSION = $(shell git ls-remote https://github.com/OpenIPC/openingenic master | awk '{ print $$1 }')
INGENIC_OPENSDK_SITE = $(call github,openipc,openingenic,$(INGENIC_OPENSDK_VERSION))
INGENIC_OPENSDK_LICENSE = GPL-3.0
INGENIC_OPENSDK_LICENSE_FILES = LICENSE

INGENIC_OPENSDK_MODULE_SUBDIRS = kernel
INGENIC_OPENSDK_MODULE_MAKE_OPTS = \
	SOC=$(OPENIPC_MODEL)

$(eval $(kernel-module))
$(eval $(generic-package))
