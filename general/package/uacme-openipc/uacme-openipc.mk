################################################################################
#
# uacme-openipc
#
################################################################################

UACME_OPENIPC_VERSION = 1.2.4
# Released versions are on branch upstream/latest, tagged as
# upstream/X.Y.Z Do not use vX.Y.Z tags from master, as they do not
# include .tarball-version
UACME_OPENIPC_SITE = $(call github,ndilieto,uacme,upstream/$(UACME_OPENIPC_VERSION))
UACME_OPENIPC_LICENSE = GPL-3.0+
UACME_OPENIPC_LICENSE_FILES = COPYING
UACME_OPENIPC_DEPENDENCIES = libcurl-openipc

UACME_OPENIPC_CONF_ENV = ac_cv_prog_cc_c99='-std=gnu99'

ifeq ($(BR2_PACKAGE_MBEDTLS_OPENIPC),y)
UACME_OPENIPC_CONF_OPTS += --with-mbedtls
UACME_OPENIPC_DEPENDENCIES += mbedtls-openipc
endif

ifeq ($(BR2_PACKAGE_UACME_OPENIPC_UALPN),y)
UACME_OPENIPC_DEPENDENCIES += libev
UACME_OPENIPC_CONF_OPTS += --with-ualpn
else
UACME_OPENIPC_CONF_OPTS += --without-ualpn
endif

$(eval $(autotools-package))
