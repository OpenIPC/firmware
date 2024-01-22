################################################################################
#
# libsrtp-openipc
#
################################################################################

LIBSRTP_OPENIPC_SITE = $(call github,cisco,libsrtp,$(LIBSRTP_OPENIPC_VERSION))
LIBSRTP_OPENIPC_VERSION = master

LIBSRTP_OPENIPC_INSTALL_STAGING = YES
LIBSRTP_OPENIPC_SUPPORTS_IN_SOURCE_BUILD = NO

LIBSRTP_OPENIPC_LICENSE = BSD-3-Clause
LIBSRTP_OPENIPC_LICENSE_FILES = LICENSE
LIBSRTP_OPENIPC_CPE_ID_VENDOR = cisco

LIBSRTP_OPENIPC_CONF_OPTS = \
	-DENABLE_OPENSSL=OFF \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE:STRING=Release

LIBSRTP_OPENIPC_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
