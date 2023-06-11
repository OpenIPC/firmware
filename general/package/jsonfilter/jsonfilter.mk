################################################################################
#
# jsonfilter | updated 2022.09.27
#
################################################################################

JSONFILTER_SITE_METHOD = git
JSONFILTER_SITE = git://git.openwrt.org/project/jsonpath
JSONFILTER_VERSION = $(shell git ls-remote $(JSONFILTER_SITE) HEAD | head -1 | awk '{ print $$1 }')

JSONFILTER_LICENSE = ISC, BSD-3-Clause

JSONFILTER_DEPENDENCIES = \
    $(if $(BR2_PACKAGE_JSON_C_OPENIPC),json-c-openipc) \
    $(if $(BR2_PACKAGE_LIBUBOX),libubox)

define JSONFILTER_INSTALL_TARGET_CMDS
    install -m 0755 -D $(@D)/jsonpath $(TARGET_DIR)/usr/bin/jsonfilter
endef

$(eval $(cmake-package))
