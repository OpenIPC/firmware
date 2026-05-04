################################################################################
#
# txw8301-openipc
#
################################################################################

TXW8301_OPENIPC_SITE_METHOD = git
TXW8301_OPENIPC_SITE = https://github.com/TXW8301/TXW8301-FMAC-linux-driver
TXW8301_OPENIPC_VERSION = 867b5a55515d10c541786f3c5c5ab6002832f8b5

TXW8301_OPENIPC_LICENSE = GPL-2.0, proprietary (hgicf firmware blob)
TXW8301_OPENIPC_REDISTRIBUTE = NO

# Module source lives under hgic_fmac/, not at the repo root.
TXW8301_OPENIPC_MODULE_SUBDIRS = hgic_fmac

# CONFIG_HGIC_SDIO / CONFIG_HGIC_USB are plain kbuild make variables, not
# Kconfig symbols. Pass them via MODULE_MAKE_OPTS so Buildroot's kernel-module
# infra forwards them to: make -C $(LINUX_DIR) M=$(@D)/hgic_fmac modules
# CONFIG_COMPILE_ERR_SUPPRESS suppresses GCC date-stamp warnings promoted to
# errors on newer toolchains (-Wno-date-time -Wno-unused-result -Wno-format).
ifeq ($(BR2_PACKAGE_TXW8301_OPENIPC_SDIO),y)
TXW8301_OPENIPC_MODULE_MAKE_OPTS = CONFIG_HGICF=m CONFIG_HGIC_SDIO=y CONFIG_COMPILE_ERR_SUPPRESS=y
TXW8301_OPENIPC_FW_BLOB = hgicf_sdio.bin
else ifeq ($(BR2_PACKAGE_TXW8301_OPENIPC_USB),y)
TXW8301_OPENIPC_MODULE_MAKE_OPTS = CONFIG_HGICF=m CONFIG_HGIC_USB=y CONFIG_COMPILE_ERR_SUPPRESS=y
TXW8301_OPENIPC_FW_BLOB = hgicf_usb.bin
endif

# GNUmakefile in tools/test_app/ places binaries under bin/$(notdir $(CC))/.
TXW8301_OPENIPC_CC_NAME = $(notdir $(TARGET_CC))

define TXW8301_OPENIPC_BUILD_CMDS
	$(MAKE) -C $(@D)/tools/test_app CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)"
endef

define TXW8301_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 644 $(TXW8301_OPENIPC_PKGDIR)/$(TXW8301_OPENIPC_FW_BLOB) \
		$(TARGET_DIR)/lib/firmware/taixin/hgicf.bin
	$(INSTALL) -D -m 644 $(TXW8301_OPENIPC_PKGDIR)/modprobe-txw8301.conf \
		$(TARGET_DIR)/etc/modprobe.d/txw8301.conf
	$(INSTALL) -D -m 644 $(TXW8301_OPENIPC_PKGDIR)/hgicf.conf.default \
		$(TARGET_DIR)/etc/hgicf.conf
	$(INSTALL) -D -m 755 $(@D)/tools/test_app/bin/$(TXW8301_OPENIPC_CC_NAME)/hgpriv \
		$(TARGET_DIR)/usr/bin/hgpriv
	$(INSTALL) -D -m 755 $(@D)/tools/test_app/bin/$(TXW8301_OPENIPC_CC_NAME)/hgicf \
		$(TARGET_DIR)/usr/bin/hgicf
	$(INSTALL) -D -m 755 $(TXW8301_OPENIPC_PKGDIR)/files/S35txw8301 \
		$(TARGET_DIR)/etc/init.d/S35txw8301
endef

$(eval $(kernel-module))
$(eval $(generic-package))
