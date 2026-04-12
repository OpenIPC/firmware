################################################################################
#
# aic8800-openipc
#
################################################################################

AIC8800_OPENIPC_SITE = $(call github,openipc,aic8800,$(AIC8800_OPENIPC_VERSION))
AIC8800_OPENIPC_VERSION = HEAD

AIC8800_OPENIPC_LICENSE = GPL-2.0

$(eval $(kernel-module))
$(eval $(generic-package))
