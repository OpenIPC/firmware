################################################################################
#
# exfat-nofuse-openipc
#
################################################################################

EXFAT_NOFUSE_OPENIPC_VERSION = 01c30ad52625a7261e1b0d874553b6ca7af25966
EXFAT_NOFUSE_OPENIPC_SITE = $(call github,dorimanx,exfat-nofuse,$(EXFAT_NOFUSE_OPENIPC_VERSION))
EXFAT_NOFUSE_OPENIPC_LICENSE = GPL-2.0
EXFAT_NOFUSE_OPENIPC_LICENSE_FILES = LICENSE

EXFAT_NOFUSE_OPENIPC_MODULE_MAKE_OPTS = CONFIG_EXFAT_FS=m

$(eval $(kernel-module))
$(eval $(generic-package))
