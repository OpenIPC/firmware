################################################################################
#
# aws-webrtc
#
################################################################################

AWS_WEBRTC_VERSION = 1.7.3
AWS_WEBRTC_SITE = $(call github,awslabs,amazon-kinesis-video-streams-webrtc-sdk-c,v$(AWS_WEBRTC_VERSION))
AWS_WEBRTC_LICENSE = Apache-2.0
AWS_WEBRTC_LICENSE_FILES = LICENSE

AWS_WEBRTC_CONF_OPTS += \
	-DBUILD_STATIC_LIBS=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DUSE_OPENSSL=OFF \
	-DUSE_MBEDTLS=ON \
	-DENABLE_DATA_CHANNEL=OFF \
	-DBUILD_DEPENDENCIES=OFF

AWS_WEBRTC_MAKE_OPTS += VERBOSE=1

AWS_WEBRTC_DEPENDENCIES = \
	libsrtp \
	mbedtls-openipc

$(eval $(cmake-package))
