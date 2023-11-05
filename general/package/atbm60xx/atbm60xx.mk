################################################################################
#
# altobeam atbm60xx wifi driver
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
ATBM60XX_SITE_METHOD = git
ATBM60XX_SITE = https://github.com/openipc/atbm_60xx
ATBM60XX_VERSION = $(shell git ls-remote $(ATBM60XX_SITE) HEAD | head -1 | cut -f1)
else
ATBM60XX_SITE = https://github.com/openipc/atbm_60xx/archive
ATBM60XX_SOURCE = master.tar.gz
endif

define ATBM60XX_INSTALL_TARGET_CMDS
        $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/atbm60xx_conf
        $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/atbm60xx_conf $(ATBM60XX_PKGDIR)/files/*.txt
endef

ATBM60XX_LICENSE = GPL-2.0
ATBM60XX_MODULE_MAKE_OPTS = \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
