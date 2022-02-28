################################################################################
#
# fdk-aac-openipc
#
################################################################################

FDK_AAC_OPENIPC_VERSION = 2.0.2
FDK_AAC_OPENIPC_SITE = $(call github,mstorsjo,fdk-aac,v$(FDK_AAC_OPENIPC_VERSION))
FDK_AAC_OPENIPC_LICENSE = fdk-aac license
FDK_AAC_OPENIPC_LICENSE_FILES = NOTICE
FDK_AAC_OPENIPC_INSTALL_STAGING = YES
FDK_AAC_OPENIPC_CONF_OPTS = --disable-static

# This package uses autoconf, but not automake, so we need to call
# their special autogen.sh script, and have custom target and staging
# installation commands.

define FDK_AAC_OPENIPC_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef
FDK_AAC_OPENIPC_PRE_CONFIGURE_HOOKS += FDK_AAC_OPENIPC_RUN_AUTOGEN

$(eval $(autotools-package))
