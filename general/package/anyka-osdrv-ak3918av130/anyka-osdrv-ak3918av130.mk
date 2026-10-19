################################################################################
#
# anyka-osdrv-ak3918av130
#
################################################################################

ANYKA_OSDRV_AK3918AV130_VERSION =
ANYKA_OSDRV_AK3918AV130_SITE =
ANYKA_OSDRV_AK3918AV130_LICENSE = PROPRIETARY

# files/kmod is a subset of driver/external/ from os/driver_normal.tar.gz
# in the AnyCloud39AV130 SDK, unmodified:
#   https://github.com/Link-WQS/ANYKA_130L_SDK
#   commit 3480d024b682ccb2cf8bcb87dc4d2ac0af4dc49e
# The SDK carries no source for the ISP, encoder, audio, Ethernet or SD
# drivers. It does for the sensors (os/driver_src/sensor), which could move to
# a from-source package like the other vendors' opensdk.
#
# The SDK builds these against its own 4.4.302-cip94 tree. The family's
# kernel (OpenIPC/linux anyka-ak3918ev300) is mainline 4.4.302 with the
# AV130 machine imported and the structures these modules share laid out the
# vendor's way; board/ak3918ev300/ak3918av130's CONFIG_LOCALVERSION="-cip94"
# gives it the release string and vermagic they were built for. That is a
# vermagic match, not a CIP kernel.
#
# Nothing at load time checks more than that string. The modules carry no
# __versions section, so CONFIG_MODVERSIONS has no CRCs to compare, and a
# layout difference would load and fail at run time. What has been checked,
# when the AV130 machine went into the kernel (OpenIPC/linux#106):
#   - every symbol all of these modules import resolves against the
#     kernel's Module.symvers;
#   - a probe module built against both the SDK's tree and this kernel
#     gives the same 275 sizes and member offsets of the structures the
#     modules use -- sk_buff, net_device, device, platform_device,
#     mmc_host/mmc_host_ops/mmc_data, i2c_adapter/i2c_client,
#     file_operations, miscdevice, v4l2_device, video_device, vb2_queue,
#     uio_info, usb_hcd, iio_dev, pwm_device, clk_hw -- after mmc_host,
#     mmc_host_ops, mmc_data, uio_info, usb_hcd, iio_dev and pwm were laid
#     out the vendor's way under MACH_AK3918AV130. gpio_chip and
#     watchdog_device still differ; no module here registers either.
# Neither is a boot. ak3918av130_lite stays out of ALL_BOARDS until these
# modules have loaded and run on an AV130 camera.
ANYKA_OSDRV_AK3918AV130_KVER = 4.4.302-cip94

# Installed under the release the built kernel reports, not the literal
# above, so modprobe always looks where the modules are; and the build stops
# when the two differ, because modules under any other release would not
# load anyway and S35modules would say nothing about it.
ANYKA_OSDRV_AK3918AV130_DEPENDENCIES = linux

define ANYKA_OSDRV_AK3918AV130_INSTALL_TARGET_CMDS
	kver=$(LINUX_VERSION_PROBED); \
	if [ "$$kver" != "$(ANYKA_OSDRV_AK3918AV130_KVER)" ]; then \
		echo "anyka-osdrv-ak3918av130: kernel release is '$$kver'," \
			"these modules were built for $(ANYKA_OSDRV_AK3918AV130_KVER)" >&2; \
		exit 1; \
	fi; \
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/$$kver/anyka && \
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$$kver/anyka \
		$(ANYKA_OSDRV_AK3918AV130_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(ANYKA_OSDRV_AK3918AV130_PKGDIR)/files/script/load_anyka
endef

$(eval $(generic-package))
