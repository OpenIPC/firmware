################################################################################
#
# ipctool | updated 2021.11.25
#
################################################################################

IPCTOOL_VERSION = 32dd1743142f625c528c00abdeeed12aec350c91
IPCTOOL_SITE = $(call github,openipc,ipctool,$(IPCTOOL_VERSION))
IPCTOOL_LICENSE = MIT
IPCTOOL_LICENSE_FILES = LICENSE

IPCTOOL_CONF_OPTS += -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DSKIP_VERSION=ON

IPCTOOL_MAKE_OPTS += VERBOSE=1

$(eval $(cmake-package))
