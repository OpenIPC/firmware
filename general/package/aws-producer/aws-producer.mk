################################################################################
#
# aws-producer
#
################################################################################

AWS_PRODUCER_SITE = $(call github,awslabs,amazon-kinesis-video-streams-producer-c,$(AWS_PRODUCER_VERSION))
AWS_PRODUCER_VERSION = HEAD

AWS_PRODUCER_INSTALL_STAGING = YES
AWS_PRODUCER_LICENSE = Apache-2.0
AWS_PRODUCER_LICENSE_FILES = BSD

AWS_PRODUCER_CONF_OPTS += \
	-DBUILD_COMMON_LWS=ON \
	-DBUILD_COMMON_CURL=OFF \
	-DBUILD_DEPENDENCIES=FALSE \
	-DUSE_OPENSSL=OFF \
	-DUSE_MBEDTLS=ON \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE=Release

define AWS_PRODUCER_POST_CONF_FIXUP
	$(Q)sed -i 's/inline/inline static/g' \
		$(@D)/dependency/libkvspic/kvspic-src/src/common/include/com/amazonaws/kinesis/video/common/CommonDefs.h \
		|| true
endef

AWS_PRODUCER_POST_CONFIGURE_HOOKS += AWS_PRODUCER_POST_CONF_FIXUP

AWS_PRODUCER_MAKE_OPTS += VERBOSE=1

AWS_PRODUCER_DEPENDENCIES = \
	libwebsockets-openipc \
	mbedtls-openipc

$(eval $(cmake-package))
