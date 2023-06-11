################################################################################
#
# libogg-openipc
#
################################################################################

LIBOGG_OPENIPC_VERSION = 1.3.5
LIBOGG_OPENIPC_SOURCE = libogg-$(LIBOGG_OPENIPC_VERSION).tar.xz
LIBOGG_OPENIPC_SITE = http://downloads.xiph.org/releases/ogg

LIBOGG_OPENIPC_LICENSE = BSD-3-Clause
LIBOGG_OPENIPC_LICENSE_FILES = COPYING

LIBOGG_OPENIPC_INSTALL_STAGING = YES
LIBOGG_OPENIPC_DEPENDENCIES = host-pkgconf

$(eval $(autotools-package))
