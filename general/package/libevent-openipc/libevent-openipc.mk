################################################################################
#
# libevent-openipc
#
################################################################################

LIBEVENT_OPENIPC_VERSION = 6f139b871100aa2322598dfd37c4145e033d2602
LIBEVENT_OPENIPC_SITE = $(call github,libevent,libevent,$(LIBEVENT_OPENIPC_VERSION))
LIBEVENT_OPENIPC_INSTALL_STAGING = YES
LIBEVENT_OPENIPC_LICENSE = BSD-3-Clause, OpenBSD
LIBEVENT_OPENIPC_LICENSE_FILES = LICENSE

# This package uses autoconf, but not automake, so we need to call
# their special autogen.sh script, and have custom target and staging
# installation commands.

define LIBEVENT_OPENIPC_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef
LIBEVENT_OPENIPC_PRE_CONFIGURE_HOOKS += LIBEVENT_OPENIPC_RUN_AUTOGEN

LIBEVENT_OPENIPC_CONF_OPTS = \
	--disable-libevent-regress \
	--disable-samples \
	--disable-thread-support \
	--disable-debug-mode \
	--disable-largefile \
	--disable-openssl
HOST_LIBEVENT_OPENIPC_CONF_OPTS = \
	--disable-libevent-regress \
	--disable-samples \
	--disable-openssl

define LIBEVENT_OPENIPC_REMOVE_PYSCRIPT
	rm $(TARGET_DIR)/usr/bin/event_rpcgen.py
endef

# libevent installs a python script to target - get rid of it if we
# don't have python support enabled
ifneq ($(BR2_PACKAGE_PYTHON)$(BR2_PACKAGE_PYTHON3),y)
LIBEVENT_OPENIPC_POST_INSTALL_TARGET_HOOKS += LIBEVENT_OPENIPC_REMOVE_PYSCRIPT
endif

ifeq ($(BR2_PACKAGE_OPENSSL),y)
LIBEVENT_OPENIPC_DEPENDENCIES += host-pkgconf openssl
LIBEVENT_OPENIPC_CONF_OPTS += --enable-openssl
else
LIBEVENT_OPENIPC_CONF_OPTS += --disable-openssl
endif

$(eval $(autotools-package))
$(eval $(host-autotools-package))
