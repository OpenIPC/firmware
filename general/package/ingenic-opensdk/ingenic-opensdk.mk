################################################################################
#
# ingenic-opensdk
#
################################################################################

INGENIC_OPENSDK_SITE_METHOD = git
INGENIC_OPENSDK_SITE = https://github.com/openipc/openingenic
INGENIC_OPENSDK_VERSION = $(shell git ls-remote $(INGENIC_OPENSDK_SITE) HEAD | head -1 | awk '{ print $$1 }')

INGENIC_OPENSDK_LICENSE = GPL-3.0
INGENIC_OPENSDK_LICENSE_FILES = LICENSE

INGENIC_OPENSDK_MODULE_SUBDIRS = kernel
INGENIC_OPENSDK_MODULE_MAKE_OPTS = \
	SOC=$(OPENIPC_SOC_MODEL)

$(eval $(kernel-module))
$(eval $(generic-package))
