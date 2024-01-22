################################################################################
#
# fdk-aac-openipc
#
################################################################################

FDK_AAC_OPENIPC_SITE = $(call github,widgetii,fdk-aac,$(FDK_AAC_OPENIPC_VERSION))
FDK_AAC_OPENIPC_VERSION = tinification

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
