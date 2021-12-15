################################################################################
#
# json-c-openipc
#
################################################################################

JSON_C_OPENIPC_VERSION = json-c-0.15-20200726
JSON_C_OPENIPC_SITE = $(call github,json-c,json-c,$(JSON_C_OPENIPC_VERSION))
JSON_C_OPENIPC_INSTALL_STAGING = YES
JSON_C_OPENIPC_LICENSE = MIT
JSON_C_OPENIPC_LICENSE_FILES = COPYING

$(eval $(cmake-package))
$(eval $(host-cmake-package))
