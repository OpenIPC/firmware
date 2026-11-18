################################################################################
#
# wireguard-tools-openipc
#
################################################################################

# v1.0.20210914, the release Buildroot 2024.02 packages. Fetched by commit
# rather than as cgit's snapshot tarball, which cgit recompresses: today's
# .tar.xz no longer matches the hash Buildroot pinned, and only Buildroot's
# mirror (keyed by its own package name) still has the original.
WIREGUARD_TOOLS_OPENIPC_SITE_METHOD = git
WIREGUARD_TOOLS_OPENIPC_SITE = https://git.zx2c4.com/wireguard-tools
WIREGUARD_TOOLS_OPENIPC_VERSION = 3ba6527130c502144e7388b900138bca6260f4e8
WIREGUARD_TOOLS_OPENIPC_LICENSE = GPL-2.0
WIREGUARD_TOOLS_OPENIPC_LICENSE_FILES = COPYING

WIREGUARD_TOOLS_OPENIPC_MAKE_OPTS = \
	WITH_SYSTEMDUNITS=no \
	WITH_BASHCOMPLETION=no \
	WITH_WGQUICK=no

define WIREGUARD_TOOLS_OPENIPC_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) $(WIREGUARD_TOOLS_OPENIPC_MAKE_OPTS) \
		-C $(@D)/src wg
endef

define WIREGUARD_TOOLS_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/src/wg $(TARGET_DIR)/usr/bin/wg
endef

$(eval $(generic-package))
