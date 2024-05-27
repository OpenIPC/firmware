#############################################################
#
# libre-openipc
#
#############################################################

LIBRE_OPENIPC_SITE = $(call github,baresip,re,$(LIBRE_OPENIPC_VERSION))
LIBRE_OPENIPC_VERSION = v3.12.0
LIBRE_OPENIPC_INSTALL_STAGING = YES

LIBRE_OPENIPC_DEPENDENCIES = mbedtls-openipc

LIBRE_OPENIPC_CONF_OPTS = \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_C_FLAGS_RELEASE="-DNDEBUG -s" \
	-DUSE_MBEDTLS=ON \
	-DLIBRE_BUILD_STATIC=OFF

$(eval $(cmake-package))
