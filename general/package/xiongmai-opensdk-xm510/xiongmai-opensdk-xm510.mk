################################################################################
#
# xiongmai-opensdk-xm510
#
################################################################################

XIONGMAI_OPENSDK_XM510_SITE = $(call github,openipc,openxiongmai,$(XIONGMAI_OPENSDK_XM510_VERSION))
XIONGMAI_OPENSDK_XM510_VERSION = 0e82ee26e74c813a7ec98496a4138d05f8c5e36f

XIONGMAI_OPENSDK_XM510_LICENSE = MIT (libwrapper), GPL-2.0 (open_mmz, open_h264 kernel modules)
XIONGMAI_OPENSDK_XM510_LICENSE_FILES = LICENSE

# Install after osdrv, as xiongmai-opensdk does with xiongmai-osdrv-xm530.
ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM510),y)
XIONGMAI_OPENSDK_XM510_DEPENDENCIES += xiongmai-osdrv-xm510
endif

# The open drivers, which load_xiongmai prefers over the vendor modules:
# drivers/xm_mmz (open_mmz.ko), the XM510 mmz.ko's allocator and /proc
# layout, and drivers/hx280enc (open_h264.ko), the encoder driver in place of
# xm510_h264.ko. The kernel-module infrastructure installs them under
# /lib/modules/<kver>/extra/. Neither needs a vendor module's symbols.
XIONGMAI_OPENSDK_XM510_MODULE_SUBDIRS = drivers/xm_mmz drivers/hx280enc

# libwrapper (uclibc symbols for the vendor binaries on musl) is this
# package's own source, not openxiongmai's.
define XIONGMAI_OPENSDK_XM510_BUILD_CMDS
	$(TARGET_CC) -fPIC -shared -s $(XIONGMAI_OPENSDK_XM510_PKGDIR)/src/lib/wrapper.c \
		-o $(@D)/libwrapper.so
endef

define XIONGMAI_OPENSDK_XM510_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/libwrapper.so $(TARGET_DIR)/lib/libwrapper.so
	ln -sfv libwrapper.so $(TARGET_DIR)/lib/libwrapper.so.0
endef

$(eval $(kernel-module))
$(eval $(generic-package))
