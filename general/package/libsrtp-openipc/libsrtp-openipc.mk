################################################################################
#
# libsrtp-openipc
#
################################################################################

LIBSRTP_OPENIPC_SITE_METHOD = git
LIBSRTP_OPENIPC_SITE = https://github.com/cisco/libsrtp
LIBSRTP_OPENIPC_VERSION = $(call EXTERNAL_SHA,$(LIBSRTP_OPENIPC_SITE),HEAD)

LIBSRTP_OPENIPC_INSTALL_STAGING = YES
LIBSRTP_OPENIPC_LICENSE = BSD-3-Clause
LIBSRTP_OPENIPC_LICENSE_FILES = LICENSE
LIBSRTP_OPENIPC_CPE_ID_VENDOR = cisco

LIBSRTP_OPENIPC_CONF_OPTS = \
	-DENABLE_OPENSSL=OFF \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE:STRING=Release

LIBSRTP_OPENIPC_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
