################################################################################
#
# jsonfilter | updated 2022.09.27
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
JSONFILTER_SITE_METHOD = git
JSONFILTER_SITE = https://github.com/openwrt/jsonpath
JSONFILTER_VERSION = $(shell git ls-remote $(JSONFILTER_SITE) HEAD | head -1 | cut -f1)
else
JSONFILTER_SITE = https://github.com/openwrt/jsonpath/archive
JSONFILTER_SOURCE = master.tar.gz
endif

JSONFILTER_LICENSE = ISC, BSD-3-Clause
JSONFILTER_DEPENDENCIES = json-c libubox

define JSONFILTER_INSTALL_TARGET_CMDS
    install -m 0755 -D $(@D)/jsonpath $(TARGET_DIR)/usr/bin/jsonfilter
endef

$(eval $(cmake-package))
