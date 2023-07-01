################################################################################
#
# jsonfilter | updated 2022.09.27
#
################################################################################

JSONFILTER_SITE_METHOD = git
JSONFILTER_SITE = https://github.com/openwrt/jsonpath
JSONFILTER_VERSION = $(call EXTERNAL_SHA,$(JSONFILTER_SITE),HEAD)

JSONFILTER_LICENSE = ISC, BSD-3-Clause

JSONFILTER_DEPENDENCIES = \
    $(if $(BR2_PACKAGE_JSON_C_OPENIPC),json-c-openipc) \
    $(if $(BR2_PACKAGE_LIBUBOX),libubox)

define JSONFILTER_INSTALL_TARGET_CMDS
    install -m 0755 -D $(@D)/jsonpath $(TARGET_DIR)/usr/bin/jsonfilter
endef

$(eval $(cmake-package))
