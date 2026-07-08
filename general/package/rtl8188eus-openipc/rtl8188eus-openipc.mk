################################################################################
#
# rtl8188eu
#
################################################################################

RTL8188EUS_OPENIPC_SITE_METHOD = git
RTL8188EUS_OPENIPC_SITE = https://github.com/aircrack-ng/rtl8188eus
RTL8188EUS_OPENIPC_VERSION = 6146193406b62e942d13d4d43580ed94ac70c218

RTL8188EUS_OPENIPC_LICENSE = GPL-2.0
RTL8188EUS_OPENIPC_LICENSE_FILES = COPYING
RTL8188EUS_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8188EU=m CONFIG_CALIBRATE_TX_POWER_TO_MAX=y

$(eval $(kernel-module))
$(eval $(generic-package))
