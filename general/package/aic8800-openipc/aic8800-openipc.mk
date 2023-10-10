################################################################################
#
# aic8800-openipc
#
################################################################################

AIC8800_OPENIPC_SITE_METHOD = git
AIC8800_OPENIPC_SITE = https://github.com/openipc/aic8800
AIC8800_OPENIPC_VERSION = $(shell git ls-remote $(AIC8800_OPENIPC_SITE) HEAD | head -1 | cut -f1)

AIC8800_OPENIPC_LICENSE = GPL-2.0

$(eval $(kernel-module))
$(eval $(generic-package))
