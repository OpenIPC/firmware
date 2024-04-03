################################################################################
#
# librws
#
################################################################################

LIBRWS_SITE = $(call github,OlehKulykov,librws,$(LIBRWS_VERSION))
LIBRWS_VERSION = 901326e962322221a83dfa4b56d02dbb2d276aa8

LIBRWS_INSTALL_STAGING = YES
LIBRWS_LICENSE = MIT
LIBRWS_LICENSE_FILES = License

#LIBRWS_CONF_OPTS += \
#	-DBUILD_COMMON_LWS=ON \
#	-DBUILD_COMMON_CURL=OFF \
#	-DBUILD_DEPENDENCIES=FALSE \
#	-DUSE_OPENSSL=OFF \
#	-DUSE_MBEDTLS=ON \
#	-DBUILD_SHARED_LIBS=OFF \
#	-DCMAKE_BUILD_TYPE=Release
#
#define LIBRWS_POST_CONF_FIXUP
#	$(Q)sed -i 's/inline/inline static/g' \
#		$(@D)/dependency/libkvspic/kvspic-src/src/common/include/com/amazonaws/kinesis/video/common/CommonDefs.h \
#		|| true
#endef

#LIBRWS_POST_CONFIGURE_HOOKS += LIBRWS_POST_CONF_FIXUP

LIBRWS_MAKE_OPTS += VERBOSE=1

#LIBRWS_DEPENDENCIES = \
#	libwebsockets-openipc \
#	mbedtls-openipc

$(eval $(cmake-package))
