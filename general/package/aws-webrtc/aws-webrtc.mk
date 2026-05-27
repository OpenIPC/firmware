################################################################################
#
# aws-webrtc
#
################################################################################

# Pinned to OpenIPC/webrtc-c bump/upstream-v1.18.1 (awslabs upstream v1.18.1,
# 2026-05-06, plus 6 OpenIPC-local commits replayed). The legacy 'develop' tip
# (frozen at 078f1677, 2022-10) is intentionally left alone so other consumers
# of the fork keep getting byte-identical bytes. The bumped SDK supports both
# mbedTLS 2.x and 3.x via MBEDTLS_VERSION_NUMBER gating; firmware's
# mbedtls-openipc (2.25.0) takes the 2.x branch.
AWS_WEBRTC_SITE = $(call github,OpenIPC,webrtc-c,$(AWS_WEBRTC_VERSION))
AWS_WEBRTC_VERSION = da92942bed42987c00a976c28b37d7e5ebcb5b85

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
