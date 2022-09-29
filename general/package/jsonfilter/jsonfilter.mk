################################################################################
#
# jsonfilter | updated 2022.09.27
#
################################################################################

# JSONFILTER_VERSION = cdc760c58077f44fc40adbbe41e1556a67c1b9a9
JSONFILTER_VERSION = c7e938d6582a436dddc938539e72dd1320625c54
JSONFILTER_SITE = git://git.openwrt.org/project/jsonpath.git
JSONFILTER_LICENSE = ISC, BSD-3-Clause

JSONFILTER_DEPENDENCIES = \
    $(if $(BR2_PACKAGE_JSON_C),json-c) \
    $(if $(BR2_PACKAGE_LIBUBOX),libubox)

define JSONFILTER_INSTALL_TARGET_CMDS
    install -m 0755 -D $(@D)/jsonpath $(TARGET_DIR)/usr/bin/jsonfilter
endef

$(eval $(cmake-package))
