################################################################################
#
# opus-openipc
#
################################################################################

OPUS_OPENIPC_VERSION = 1.3.1
OPUS_OPENIPC_SOURCE = opus-$(OPUS_OPENIPC_VERSION).tar.gz
OPUS_OPENIPC_SITE = https://downloads.xiph.org/releases/opus
OPUS_OPENIPC_DL_SUBDIR = opus
OPUS_OPENIPC_LICENSE = BSD-3-Clause
OPUS_OPENIPC_LICENSE_FILES = COPYING
OPUS_OPENIPC_INSTALL_STAGING = YES

OPUS_OPENIPC_CFLAGS = $(TARGET_CFLAGS)

# Optional HiSilicon ot_opus_* shim. Drops src/ot_opus_shim.c into the
# tree, edits Makefile.am to add it to libopus_la_SOURCES, and turns on
# autoreconf so Makefile.in picks up the change. Only enabled when the
# closed vendor userspace on the target requires the ot_opus_* surface
# (Hi3516CV6xx). Off by default, so other SoCs neither pull host
# autotools deps nor pay the 6 dead symbols.
ifeq ($(BR2_PACKAGE_OPUS_OPENIPC_HISI_SHIM),y)
OPUS_OPENIPC_AUTORECONF = YES
define OPUS_OPENIPC_ADD_HISI_SHIM
	mkdir -p $(@D)/src
	cp $(OPUS_OPENIPC_PKGDIR)/src/ot_opus_shim.c $(@D)/src/ot_opus_shim.c
	$(SED) 's|^libopus_la_SOURCES = \$$(CELT_SOURCES) \$$(SILK_SOURCES) \$$(OPUS_SOURCES)$$|& src/ot_opus_shim.c|' \
		$(@D)/Makefile.am
endef
OPUS_OPENIPC_POST_EXTRACT_HOOKS += OPUS_OPENIPC_ADD_HISI_SHIM
endif

ifeq ($(BR2_TOOLCHAIN_HAS_GCC_BUG_85180),y)
OPUS_OPENIPC_CFLAGS += -O0
endif

OPUS_OPENIPC_CONF_ENV = CFLAGS="$(OPUS_OPENIPC_CFLAGS)"

ifeq ($(BR2_PACKAGE_OPUS_OPENIPC_FIXED_POINT),y)
OPUS_OPENIPC_CONF_OPTS += --enable-fixed-point
endif

# When we're on ARM, but we don't have ARM instructions (only
# Thumb-2), disable the usage of assembly as it is not Thumb-ready.
ifeq ($(BR2_arm)$(BR2_armeb):$(BR2_ARM_CPU_HAS_ARM),y:)
OPUS_OPENIPC_CONF_OPTS += --disable-asm
endif

$(eval $(autotools-package))
