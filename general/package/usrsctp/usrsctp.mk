################################################################################
#
# usrsctp
#
################################################################################

USRSCTP_SITE_METHOD = git
USRSCTP_SITE = https://github.com/sctplab/usrsctp
USRSCTP_VERSION = 1ade45cbadfd19298d2c47dc538962d4425ad2dd

USRSCTP_INSTALL_STAGING = YES
USRSCTP_LICENSE = Apache-2.0
USRSCTP_LICENSE_FILES = BSD

USRSCTP_CONF_OPTS += \
	-Dsctp_werror=0 \
	-Dsctp_build_programs=0 \
	-Dsctp_debug=0 \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE=Release

USRSCTP_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
