################################################################################
#
# libsrt
#
################################################################################

LIBSRT_VERSION = e932e8fbdda4a053023985a6f9c076cf09810e98
LIBSRT_SITE = $(call github,Haivision,srt,$(LIBSRT_VERSION))
LIBSRT_LICENSE = MPLv2.0
LIBSRT_LICENSE_FILES = LICENSE

LIBSRT_CONF_OPTS += --disable-encryption --disable-apps

LIBSRT_MAKE_OPTS += VERBOSE=1

$(eval $(autotools-package))

