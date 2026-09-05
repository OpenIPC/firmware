################################################################################
#
# libevent-openipc
#
################################################################################

LIBEVENT_OPENIPC_SITE_METHOD = git
LIBEVENT_OPENIPC_SITE = https://github.com/OpenIPC/libevent
# Branch 'openipc': upstream master, plus the two pull requests libevent has
# not reviewed -- #1867, which adds the EVENT__DISABLE_* toggles this package
# builds with, and #1865 -- plus the BROKEN_MMAP shim this package used to
# carry as patch 0002, plus the evws send-after-close fix (OpenIPC/libevent#2).
# The upstream pull requests stay open there; when one lands, the commit drops
# out of the branch rather than out of this file.
#
# By commit, not by branch. This package used to track the pull request
# directly, as refs/pull/1867/head on a repository nobody here controls. That
# ref is resolved at download time, the author rebased it on 2026-08-29, and
# every clean build of the boards selecting this package began failing on a
# patch that no longer applied -- with nothing in this tree having changed.
# Owning the repository takes the force-push out of a stranger's hands;
# pinning the commit takes the resolve-at-download-time out of the build.
LIBEVENT_OPENIPC_VERSION = 694decef35717d8955aa34ba4d2baaaf61c9e4a9

LIBEVENT_OPENIPC_INSTALL_STAGING = YES
LIBEVENT_OPENIPC_LICENSE = BSD-3-Clause, OpenBSD
LIBEVENT_OPENIPC_LICENSE_FILES = LICENSE
LIBEVENT_OPENIPC_CONF_OPTS = \
	-D_GNU_SOURCE=ON \
	-DEVENT__DISABLE_BENCHMARK=ON \
	-DEVENT__DISABLE_SAMPLES=ON \
	-DEVENT__DISABLE_TESTS=ON \
	-DEVENT__DISABLE_RPC=ON \
	-DEVENT__DISABLE_EVENT_TAGGING=ON \
	-DCMAKE_BUILD_TYPE=Release

define LIBEVENT_OPENIPC_REMOVE_PYSCRIPT
	rm $(TARGET_DIR)/usr/bin/event_rpcgen.py
endef

# The build emits both the split libraries -- core, extra, pthreads, mbedtls,
# which are what everything here links -- and a monolithic libevent-2.2.so that
# nothing links at all. Delete the monolith by glob rather than by literal
# soversion: it was removed as libevent-2.2.so.1.0.0 until upstream went to
# 1.0.1, after which 243KB of orphan rode into every image unnoticed and put
# hi3516av300_neo 52KB past its 8192KB rootfs cap.
define LIBEVENT_OPENIPC_DELETE_UNUSED
	rm -f $(TARGET_DIR)/usr/lib/libevent-2.2.so*
	rm -f $(TARGET_DIR)/usr/lib/libevent.so
endef

# libevent installs a python script to target - get rid of it if we
# don't have python support enabled
ifneq ($(BR2_PACKAGE_PYTHON)$(BR2_PACKAGE_PYTHON3),y)
LIBEVENT_OPENIPC_POST_INSTALL_TARGET_HOOKS += LIBEVENT_OPENIPC_REMOVE_PYSCRIPT
endif

ifeq ($(BR2_PACKAGE_OPENSSL),y)
LIBEVENT_OPENIPC_DEPENDENCIES += host-pkgconf openssl
LIBEVENT_OPENIPC_CONF_OPTS += -DEVENT__DISABLE_OPENSSL=OFF
else
LIBEVENT_OPENIPC_CONF_OPTS += -DEVENT__DISABLE_OPENSSL=ON
endif

ifeq ($(BR2_PACKAGE_MBEDTLS_OPENIPC),y)
LIBEVENT_OPENIPC_DEPENDENCIES += host-pkgconf mbedtls-openipc
LIBEVENT_OPENIPC_CONF_OPTS += -DEVENT__DISABLE_MBEDTLS=OFF
else
LIBEVENT_OPENIPC_CONF_OPTS += -DEVENT__DISABLE_MBEDTLS=ON
endif

# BROKEN_MMAP makes buffer.c issue syscall(SYS_mmap2,...) instead of the libc
# mmap() that fails on 32-bit musl. SYS_mmap2 is 32-bit-ARM-only (aarch64 has
# SYS_mmap), and aarch64 musl mmap() works fine, so only ask for it on 32-bit.
ifeq ($(BR2_TOOLCHAIN_USES_MUSL),y)
ifneq ($(BR2_aarch64),y)
LIBEVENT_OPENIPC_CONF_OPTS += -DCMAKE_C_FLAGS="$(TARGET_CFLAGS) -DBROKEN_MMAP=1"
endif
endif

LIBEVENT_OPENIPC_POST_INSTALL_TARGET_HOOKS += LIBEVENT_OPENIPC_DELETE_UNUSED

$(eval $(cmake-package))
