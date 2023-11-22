################################################################################
#
# hisilicon-osdrv-serdes
#
################################################################################

HISILICON_OSDRV_SERDES_SITE_METHOD = local
HISILICON_OSDRV_SERDES_SITE = $(HISILICON_OSDRV_SERDES_PKGDIR)/src

HISILICON_OSDRV_SERDES_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
