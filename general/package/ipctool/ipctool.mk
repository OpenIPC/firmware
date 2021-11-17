################################################################################
#
# ipctool | updated 2021.11.17
#
################################################################################

IPCTOOL_VERSION = 7813c366ebbef2476826d8167c8e024eb448bd6c
IPCTOOL_SITE = $(call github,openipc,ipctool,$(IPCTOOL_VERSION))
IPCTOOL_LICENSE = MIT
IPCTOOL_LICENSE_FILES = LICENSE

IPCTOOL_CONF_OPTS += -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DSKIP_VERSION=ON

IPCTOOL_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
