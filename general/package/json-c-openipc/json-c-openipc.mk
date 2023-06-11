################################################################################
#
# json-c-openipc
#
################################################################################

JSON_C_OPENIPC_SITE_METHOD = git
JSON_C_OPENIPC_SITE = https://github.com/json-c/json-c
JSON_C_OPENIPC_VERSION = $(shell git ls-remote $(JSON_C_OPENIPC_SITE) json-c-0.15-20200726 | head -1 | awk '{ print $$1 }')

JSON_C_OPENIPC_INSTALL_STAGING = YES
JSON_C_OPENIPC_LICENSE = MIT
JSON_C_OPENIPC_LICENSE_FILES = COPYING

$(eval $(cmake-package))
$(eval $(host-cmake-package))
