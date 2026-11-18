################################################################################
#
# ssw101b
#
################################################################################

SSW101B_SITE = $(call github,openipc,ssw101b,$(SSW101B_VERSION))
# Pinned rather than HEAD: 1b3ec47 is the GNU make 4.4 fix (OpenIPC/ssw101b#2).
# Its bare exports handed kbuild's multi_depend to $(shell), whose expansion
# ran an $(eval) mid-parse, and ssc333/ssc335 ultimate stopped building on
# ubuntu-26.04 with "prerequisites cannot be defined in recipes".
SSW101B_VERSION = 1b3ec47e46d44cc8ab4688e01a3f6f747199f965

SSW101B_LICENSE = GPL-2.0

$(eval $(kernel-module))
$(eval $(generic-package))
