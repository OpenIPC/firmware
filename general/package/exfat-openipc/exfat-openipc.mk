################################################################################
#
# exfat-openipc
#
################################################################################

EXFAT_OPENIPC_VERSION = 7f592ba5718e3e757cd0edd2d934fe2fe87f7d61
EXFAT_OPENIPC_SITE = $(call github,namjaejeon,linux-exfat-oot,$(EXFAT_OPENIPC_VERSION))
EXFAT_OPENIPC_LICENSE = GPL-2.0
EXFAT_OPENIPC_LICENSE_FILES = COPYING

EXFAT_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_EXFAT_FS=m \
	KDIR=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
