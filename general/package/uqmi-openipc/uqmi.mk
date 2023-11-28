################################################################################
#
# uqmi-openipc
#
################################################################################

UQMI_OPENIPC_VERSION = f254fc59c710d781eca3ec36e0bff2d8970370fa
UQMI_OPENIPC_SITE = git://git.openwrt.org/project/uqmi.git

UQMI_OPENIPC_LICENSE = LGPL-2.0+
UQMI_OPENIPC_LICENSE_FILES = COPYING
UQMI_OPENIPC_DEPENDENCIES = json-c libubox

$(eval $(cmake-package))
