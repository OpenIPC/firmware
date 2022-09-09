################################################################################
#
# uqmi-openipc
#
################################################################################

UQMI_VERSION = f254fc59c710d781eca3ec36e0bff2d8970370fa
UQMI_SITE = git://git.openwrt.org/project/uqmi.git
UQMI_DEPENDENCIES = json-c-openipc libubox
UQMI_LICENSE = LGPL-2.0+
UQMI_LICENSE_FILES = COPYING

$(eval $(cmake-package))
