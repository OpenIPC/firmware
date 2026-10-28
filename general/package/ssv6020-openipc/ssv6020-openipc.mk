################################################################################
#
# ssv6020-openipc
#
################################################################################

# OpenIPC/ssv6x5x's ssv6020/ directory: iComm's L.SMAC trunk r170 with the
# SSV6020 HAL, as Allwinner's BSP ships it for the SSV6158, built as an
# external module on 3.10 kernels.
SSV6020_OPENIPC_SITE = $(call github,openipc,ssv6x5x,$(SSV6020_OPENIPC_VERSION))
SSV6020_OPENIPC_VERSION = 1d5db4649d12cdfddc7c8ecccd039e528afca89f

SSV6020_OPENIPC_LICENSE = GPL-2.0

SSV6020_OPENIPC_MODULE_SUBDIRS = ssv6020
SSV6020_OPENIPC_MODULE_MAKE_OPTS = CONFIG_SSV6X5X=m

define SSV6020_OPENIPC_INSTALL_FIRMWARE
	$(INSTALL) -D -m 644 $(@D)/ssv6020/image/ssv6x5x-sw.bin $(TARGET_DIR)/lib/firmware/ssv6x5x-sw.bin
	$(INSTALL) -D -m 644 $(@D)/ssv6020/ssv6x5x-wifi.cfg $(TARGET_DIR)/etc/ssv6x5x-wifi.cfg
endef
SSV6020_OPENIPC_POST_INSTALL_TARGET_HOOKS += SSV6020_OPENIPC_INSTALL_FIRMWARE

$(eval $(kernel-module))
$(eval $(generic-package))
