################################################################################
# ARC firmware assembler and linker
################################################################################

# Buildroot's binutils, so the version moves with each Buildroot bump
HOST_ARC_BINUTILS_VERSION = $(BINUTILS_VERSION)
HOST_ARC_BINUTILS_SOURCE = $(BINUTILS_SOURCE)
HOST_ARC_BINUTILS_SITE = $(BINUTILS_SITE)
HOST_ARC_BINUTILS_LICENSE = $(BINUTILS_LICENSE)
HOST_ARC_BINUTILS_LICENSE_FILES = $(BINUTILS_LICENSE_FILES)
HOST_ARC_BINUTILS_DEPENDENCIES = host-zlib
HOST_ARC_BINUTILS_CONF_ENV = MAKEINFO=true
HOST_ARC_BINUTILS_CONF_OPTS = \
	--target=arc-elf32 \
	--disable-binutils \
	--disable-gdb \
	--disable-gold \
	--disable-gprof \
	--disable-gprofng \
	--disable-multilib \
	--disable-nls \
	--disable-plugins \
	--disable-shared \
	--disable-sim \
	--disable-werror \
	--without-debuginfod \
	--without-zstd \
	--with-system-zlib

define HOST_ARC_BINUTILS_BUILD_CMDS
	$(HOST_MAKE_ENV) $(MAKE) -C $(@D) MAKEINFO=true all-gas all-ld
endef

define HOST_ARC_BINUTILS_INSTALL_CMDS
	$(HOST_MAKE_ENV) $(MAKE) -C $(@D) MAKEINFO=true install-gas install-ld
endef

$(eval $(host-autotools-package))
