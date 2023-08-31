################################################################################
#
# exfat-openipc
#
################################################################################

EXFAT_OPENIPC_SITE_METHOD = git
EXFAT_OPENIPC_SITE = https://github.com/namjaejeon/linux-exfat-oot
EXFAT_OPENIPC_VERSION = 2a0023bc28e080f2317a4eb293cf28c3126948de

EXFAT_OPENIPC_LICENSE = GPL-2.0
EXFAT_OPENIPC_LICENSE_FILES = COPYING

EXFAT_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_EXFAT_FS=m \
	KDIR=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
