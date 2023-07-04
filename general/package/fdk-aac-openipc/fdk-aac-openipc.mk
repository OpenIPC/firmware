################################################################################
#
# fdk-aac-openipc
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
FDK_AAC_OPENIPC_SITE_METHOD = git
FDK_AAC_OPENIPC_SITE = https://github.com/widgetii/fdk-aac
FDK_AAC_OPENIPC_VERSION = $(shell git ls-remote $(FDK_AAC_OPENIPC_SITE) tinification | head -1 | cut -f1)
else
FDK_AAC_OPENIPC_SITE = https://github.com/widgetii/fdk-aac/archive
FDK_AAC_OPENIPC_SOURCE = tinification.tar.gz
endif

FDK_AAC_OPENIPC_LICENSE = fdk-aac license
FDK_AAC_OPENIPC_LICENSE_FILES = NOTICE
FDK_AAC_OPENIPC_INSTALL_STAGING = YES
FDK_AAC_OPENIPC_CONF_OPTS = \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE \
	-DCMAKE_POLICY_DEFAULT_CMP0069=NEW \
	-DDISABLE_SBR_ENCODER=ON \
	-DDISABLE_SAC_ENCODER=ON \
	-DDISABLE_META_ENCODER=ON \
	-DDISABLE_NOISE_SHAPING=ON \
	-DDISABLE_TRANSPORT_ENCODER=ON \
	-DDISABLE_STEREO=ON \
	-DDISABLE_DECODERS=ON \
	-DCMAKE_BUILD_TYPE=Release

$(eval $(cmake-package))
