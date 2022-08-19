################################################################################
#
# aws-producer
#
################################################################################

AWS_PRODUCER_VERSION = c7fce9e06021452ff3c42dc70c8360606b22ad53
AWS_PRODUCER_SITE = https://github.com/awslabs/amazon-kinesis-video-streams-producer-c.git
AWS_PRODUCER_SITE_METHOD = git
AWS_PRODUCER_INSTALL_STAGING = YES
AWS_PRODUCER_LICENSE = Apache-2.0
AWS_PRODUCER_LICENSE_FILES = BSD

AWS_PRODUCER_CONF_OPTS += \
	-DBUILD_COMMON_LWS=ON \
	-DBUILD_COMMON_CURL=OFF \
	-DBUILD_DEPENDENCIES=FALSE \
	-DUSE_OPENSSL=OFF \
	-DUSE_MBEDTLS=ON \
	-DCMAKE_BUILD_TYPE=Release

AWS_PRODUCER_MAKE_OPTS += VERBOSE=1

AWS_PRODUCER_DEPENDENCIES = \
	libwebsockets-openipc \
	mbedtls-openipc

$(eval $(cmake-package))
