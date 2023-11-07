################################################################################
#
# ssw101b
#
################################################################################

SSW101B_SITE = https://github.com/openipc/ssw101b/archive
SSW101B_SOURCE = master.tar.gz
SSW101B_LICENSE = GPL-2.0

$(eval $(kernel-module))
$(eval $(generic-package))
