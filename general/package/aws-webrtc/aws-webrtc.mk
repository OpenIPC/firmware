################################################################################
#
# aws-webrtc
#
################################################################################

AWS_WEBRTC_SITE_METHOD = git
AWS_WEBRTC_SITE = https://github.com/OpenIPC/webrtc-c
AWS_WEBRTC_VERSION = $(shell git ls-remote $(AWS_WEBRTC_SITE) develop | head -1 | cut -f1)

AWS_WEBRTC_INSTALL_STAGING = YES
AWS_WEBRTC_LICENSE = Apache-2.0
AWS_WEBRTC_LICENSE_FILES = LICENSE

AWS_WEBRTC_CONF_OPTS += \
	-DBUILD_STATIC_LIBS=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DUSE_OPENSSL=OFF \
	-DUSE_MBEDTLS=ON \
	-DENABLE_DATA_CHANNEL=OFF \
	-DBUILD_SAMPLE=OFF \
	-DBUILD_DEPENDENCIES=OFF

AWS_WEBRTC_MAKE_OPTS += VERBOSE=1

AWS_WEBRTC_DEPENDENCIES = \
	libsrtp-openipc \
	aws-producer \
	usrsctp \
	mbedtls-openipc

$(eval $(cmake-package))
