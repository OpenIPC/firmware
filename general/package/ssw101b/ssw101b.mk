################################################################################
#
# ssw101b
#
################################################################################

SSW101B_SITE = $(call github,openipc,ssw101b,$(SSW101B_VERSION))
SSW101B_VERSION = HEAD

SSW101B_LICENSE = GPL-2.0

$(eval $(kernel-module))
$(eval $(generic-package))
