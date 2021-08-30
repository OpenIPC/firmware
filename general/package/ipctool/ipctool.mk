################################################################################
#
# ipctool | updated 2021.08.30
#
################################################################################

IPCTOOL_VERSION = 7f97012a950aea758d2df827a0772dacc4c8bc99
IPCTOOL_SITE = $(call github,openipc,ipctool,$(IPCTOOL_VERSION))
IPCTOOL_LICENSE = MIT
IPCTOOL_LICENSE_FILES = LICENSE

IPCTOOL_CONF_OPTS += -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DSKIP_VERSION=ON

IPCTOOL_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
