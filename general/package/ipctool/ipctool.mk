################################################################################
#
# ipctool | updated 2021.09.02
#
################################################################################

IPCTOOL_VERSION = cfa2c8329419a695e055f2e22d1c37ccd5daa80f
IPCTOOL_SITE = $(call github,openipc,ipctool,$(IPCTOOL_VERSION))
IPCTOOL_LICENSE = MIT
IPCTOOL_LICENSE_FILES = LICENSE

IPCTOOL_CONF_OPTS += -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DSKIP_VERSION=ON

IPCTOOL_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
