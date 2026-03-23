################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# n3n-openipc — n3n-openipc Buildroot package
#
################################################################################

N3N_OPENIPC_SITE = $(call github,n42n,n3n,$(N3N_OPENIPC_VERSION))
N3N_OPENIPC_VERSION = 44b93f02f30a12187a6802b0268f614c08b41077
# N3N_OPENIPC_VERSION = HEAD

N3N_OPENIPC_LICENSE = GPL-2.0
N3N_OPENIPC_LICENSE_FILES = LICENSE
N3N_OPENIPC_INSTALL_STAGING = YES

N3N_OPENIPC_CFLAGS = $(TARGET_CFLAGS) -fno-strict-aliasing

define N3N_OPENIPC_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef

N3N_OPENIPC_PRE_CONFIGURE_HOOKS += N3N_OPENIPC_RUN_AUTOGEN

#N3N_OPENIPC_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
N3N_OPENIPC_MAKE_OPTS += VERBOSE=1

define N3N_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(N3N_OPENIPC_PKGDIR)/files/S89edge

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/apps/n3n-edge
#	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(@D)/apps/n3n-supernode
endef

$(eval $(autotools-package))
