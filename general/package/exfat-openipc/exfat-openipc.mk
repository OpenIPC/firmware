################################################################################
#
# exfat-openipc
#
################################################################################

EXFAT_OPENIPC_SITE_METHOD = git
EXFAT_OPENIPC_SITE = https://github.com/namjaejeon/linux-exfat-oot
EXFAT_OPENIPC_VERSION = 331d3f5c425717085a825253f6aeb99af6848099

EXFAT_OPENIPC_LICENSE = GPL-2.0
EXFAT_OPENIPC_LICENSE_FILES = COPYING

EXFAT_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_EXFAT_FS=m \
	KDIR=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
