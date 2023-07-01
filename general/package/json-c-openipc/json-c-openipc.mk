################################################################################
#
# json-c-openipc
#
################################################################################

JSON_C_OPENIPC_SITE_METHOD = git
JSON_C_OPENIPC_SITE = https://github.com/json-c/json-c
JSON_C_OPENIPC_VERSION = 9021cdcdd01fc9dbcbe1f06391848c2ac915212f

JSON_C_OPENIPC_INSTALL_STAGING = YES
JSON_C_OPENIPC_LICENSE = MIT
JSON_C_OPENIPC_LICENSE_FILES = COPYING

$(eval $(cmake-package))
$(eval $(host-cmake-package))
