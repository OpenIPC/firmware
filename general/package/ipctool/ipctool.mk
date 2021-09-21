################################################################################
#
# ipctool | updated 2021.09.21
#
################################################################################

IPCTOOL_VERSION = 398210dc673fcb0741281b3bd03cae5731c66985
IPCTOOL_SITE = $(call github,openipc,ipctool,$(IPCTOOL_VERSION))
IPCTOOL_LICENSE = MIT
IPCTOOL_LICENSE_FILES = LICENSE

IPCTOOL_CONF_OPTS += -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DSKIP_VERSION=ON

IPCTOOL_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
