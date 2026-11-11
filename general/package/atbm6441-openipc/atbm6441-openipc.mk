################################################################################
#
# atbm6441-openipc
#
################################################################################

ATBM6441_OPENIPC_SITE = $(call github,openipc,atbm6441,$(ATBM6441_OPENIPC_VERSION))
ATBM6441_OPENIPC_VERSION = 5cd383d4dbef9b3fc035e53db5671430faaf5983

# The README says proprietary; the sources carry GPL-2.0 headers (the
# cw1200 driver they derive from) and MODULE_LICENSE("GPL").
ATBM6441_OPENIPC_LICENSE = GPL-2.0
ATBM6441_OPENIPC_DEPENDENCIES = linux

# Makefile.build carries the chip configuration (HERA, SDIO, no TX
# confirm) and exports it to the kbuild pass, which reads nothing else.
define ATBM6441_OPENIPC_BUILD_CMDS
	$(MAKE) -C $(@D) -f $(@D)/Makefile.build modules \
		$(LINUX_MAKE_FLAGS) KDIR=$(LINUX_DIR) \
		MODULES_NAME=atbm6441_wifi_sdio \
		ATBM_WIFI__EXT_CCFLAGS="-DATBM_WIFI_PLATFORM=22"
endef

define ATBM6441_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 644 $(@D)/hal_apollo/atbm6441_wifi_sdio.ko \
		$(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/extra/atbm6441_wifi_sdio.ko
	$(TARGET_CROSS)strip --strip-debug \
		$(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/extra/atbm6441_wifi_sdio.ko
endef

$(eval $(generic-package))
