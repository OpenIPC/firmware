################################################################################
#
# vtund-lite
#
################################################################################

VTUND_LITE_VERSION = 3.0.4
VTUND_LITE_SITE = http://downloads.sourceforge.net/project/vtun/vtun/$(VTUND_LITE_VERSION)
VTUND_LITE_LICENSE = GPL-2.0+ with OpenSSL exception
VTUND_LITE_LICENSE_FILES = README.OpenSSL
VTUND_LITE_DEPENDENCIES = host-bison host-flex host-pkgconf
VTUND_LITE_AUTORECONF = YES

VTUND_LITE_CONF_OPTS = \
	--without-ssl-headers=$(STAGING_DIR)/usr/include/openssl \
	--without-lzo-headers=$(STAGING_DIR)/usr/include/lzo \
	--without-lzo-lib=$(STAGING_DIR)/usr/lib

# Assumes old-style gcc inline symbol visibility rules
VTUND_LITE_CONF_ENV = CFLAGS="$(TARGET_CFLAGS) -std=gnu89"

# configure.in forgets to link to dependent libraries of openssl breaking static
# linking
VTUND_LITE_CONF_ENV += LIBS=`$(PKG_CONFIG_HOST_BINARY)`

$(eval $(autotools-package))
