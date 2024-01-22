################################################################################
#
# jsonfilter
#
################################################################################

JSONFILTER_SITE = $(call github,openwrt,jsonpath,$(JSONFILTER_VERSION))
JSONFILTER_VERSION = master

JSONFILTER_LICENSE = ISC, BSD-3-Clause
JSONFILTER_DEPENDENCIES = json-c libubox

define JSONFILTER_INSTALL_TARGET_CMDS
    install -m 0755 -D $(@D)/jsonpath $(TARGET_DIR)/usr/bin/jsonfilter
endef

$(eval $(cmake-package))
