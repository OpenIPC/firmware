################################################################################
#
# xiongmai-opensdk-xm510
#
################################################################################

XIONGMAI_OPENSDK_XM510_SITE = $(call github,openipc,openxiongmai,$(XIONGMAI_OPENSDK_XM510_VERSION))
XIONGMAI_OPENSDK_XM510_VERSION = 449416b044ee0dd794d17c969e52456f23214abe

XIONGMAI_OPENSDK_XM510_LICENSE = MIT (libwrapper), GPL-2.0 (open_mmz, open_h264, open_vi, open_isp kernel modules)
XIONGMAI_OPENSDK_XM510_LICENSE_FILES = LICENSE

# Install after osdrv, as xiongmai-opensdk does with xiongmai-osdrv-xm530.
ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM510),y)
XIONGMAI_OPENSDK_XM510_DEPENDENCIES += xiongmai-osdrv-xm510
endif

# The open drivers, which load_xiongmai prefers over the vendor modules:
# drivers/xm_mmz (open_mmz.ko), the XM510 mmz.ko's allocator and /proc
# layout; drivers/hx280enc (open_h264.ko), the encoder driver in place of
# xm510_h264.ko; and drivers/xm510_vi (open_vi.ko), the VI driver in place of
# xm510_vi.ko, which releases the frames of a consumer that exits; and
# drivers/xm510_isp (open_isp.ko), the ISP device in place of xm510_isp.ko,
# without its "Bad page state" on unload. The kernel-module infrastructure
# installs them under /lib/modules/<kver>/extra/. None has a module
# dependency: open_vi looks the MMZ symbols up.
XIONGMAI_OPENSDK_XM510_MODULE_SUBDIRS = drivers/xm_mmz drivers/hx280enc drivers/xm510_vi drivers/xm510_isp

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
