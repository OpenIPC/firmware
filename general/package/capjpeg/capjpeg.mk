################################################################################
#
# capjpeg
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
CAPJPEG_SITE_METHOD = git
CAPJPEG_SITE = https://github.com/openipc/capjpeg
CAPJPEG_VERSION = $(shell git ls-remote $(CAPJPEG_SITE) HEAD | head -1 | cut -f1)
else
CAPJPEG_SITE = https://github.com/openipc/capjpeg/archive
CAPJPEG_SOURCE = main.tar.gz
endif

CAPJPEG_LICENSE = MIT
CAPJPEG_LICENSE_FILES = LICENSE

define CAPJPEG_INSTALL_TARGET_CMDS
$(INSTALL) -m 0755 -D $(@D)/capJPEG $(TARGET_DIR)/usr/bin/capjpeg
endef

$(eval $(generic-package))
