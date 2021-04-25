################################################################################
#
# libevent-mini
#
################################################################################

LIBEVENT_MINI_VERSION = 2.1.12
LIBEVENT_MINI_SITE = https://github.com/libevent/libevent/releases/download/release-$(LIBEVENT_MINI_VERSION)-stable
LIBEVENT_MINI_SOURCE = libevent-$(LIBEVENT_MINI_VERSION)-stable.tar.gz
LIBEVENT_MINI_INSTALL_STAGING = YES
LIBEVENT_MINI_LICENSE = BSD-3-Clause, OpenBSD
LIBEVENT_MINI_LICENSE_FILES = LICENSE
LIBEVENT_MINI_CONF_OPTS = \
	--disable-thread-support \
	--disable-debug-mode \
	--disable-largefile \
	--disable-libevent-regress \
	--disable-samples \
	--disable-openssl

define LIBEVENT_MINI_REMOVE_PYSCRIPT
	rm $(TARGET_DIR)/usr/bin/event_rpcgen.py
endef

$(eval $(autotools-package))
$(eval $(host-autotools-package))
