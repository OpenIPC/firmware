################################################################################
#
# lame-openipc
#
################################################################################

LAME_OPENIPC_VERSION = 3.100
LAME_OPENIPC_SOURCE = lame-$(LAME_OPENIPC_VERSION).tar.gz
LAME_OPENIPC_SITE = http://prdownloads.sourceforge.net/lame
LAME_OPENIPC_DEPENDENCIES = host-pkgconf
LAME_OPENIPC_INSTALL_STAGING = YES
LAME_OPENIPC_CONF_ENV = GTK_CONFIG=/bin/false
LAME_OPENIPC_CONF_OPTS = --disable-frontend --disable-decoder --disable-gtktest
LAME_OPENIPC_LICENSE = LGPL-2.0+
LAME_OPENIPC_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_LIBSNDFILE),y)
LAME_OPENIPC_DEPENDENCIES += libsndfile
LAME_OPENIPC_CONF_OPTS += --with-fileio=sndfile
endif

ifeq ($(BR2_PACKAGE_NCURSES),y)
LAME_OPENIPC_DEPENDENCIES += ncurses
endif

ifeq ($(BR2_ENDIAN),"BIG")
define LAME_OPENIPC_BIGENDIAN_ARCH
	echo "#define WORDS_BIGENDIAN 1" >>$(@D)/config.h
endef
endif

LAME_OPENIPC_POST_CONFIGURE_HOOKS += LAME_OPENIPC_BIGENDIAN_ARCH

$(eval $(autotools-package))
