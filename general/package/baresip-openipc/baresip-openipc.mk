#############################################################
#
# baresip-openipc
#
#############################################################

BARESIP_OPENIPC_SITE = $(call github,baresip,baresip,$(BARESIP_OPENIPC_VERSION))
BARESIP_OPENIPC_VERSION = v3.12.0

BARESIP_OPENIPC_DEPENDENCIES = libre-openipc

BARESIP_OPENIPC_CONF_OPTS = \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_C_FLAGS_RELEASE="-DNDEBUG -s" \
	-DUSE_MBEDTLS=ON

$(eval $(cmake-package))
