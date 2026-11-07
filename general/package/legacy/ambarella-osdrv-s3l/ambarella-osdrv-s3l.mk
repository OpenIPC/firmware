################################################################################
#
# ambarella-osdrv-s3l
#
################################################################################

AMBARELLA_OSDRV_S3L_SITE_METHOD = local
AMBARELLA_OSDRV_S3L_SITE = $(AMBARELLA_OSDRV_S3L_PKGDIR)/src
AMBARELLA_OSDRV_S3L_LICENSE = MIT (load_ucode), PROPRIETARY (modules, microcode)

# load_ucode puts the DSP microcode in place. files/firmware is the generic set
# from Ambarella's S3L SDK release 2.5.0_20181109 (2018/10/28, 284712.282649),
# not from a camera's stock firmware, which the tree does not accept (#2767).
#
# The prebuilt modules hard-code struct offsets and carry no __versions, so a
# kernel config that moves them builds, boots and loads the modules, and fails
# only at the first /dev/iav ioctl. iav_open reads inode->i_cdev at 272 and
# writes file->private_data at 112: CONFIG_LBDAF moves the first (#2763), and
# CONFIG_QUOTA on top of it moved it again (#2778). So the kernel's own Kbuild
# compiles src/abi against this kernel, and the build stops on a mismatch.
AMBARELLA_OSDRV_S3L_DEPENDENCIES = linux
AMBARELLA_OSDRV_S3L_IAV_OFFSETS = INODE_I_CDEV:272 FILE_PRIVATE_DATA:112

define AMBARELLA_OSDRV_S3L_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) load_ucode
	$(LINUX_MAKE_ENV) $(MAKE) -C $(LINUX_DIR) $(LINUX_MAKE_FLAGS) \
		M=$(@D)/abi abi_probe.s
	for want in $(AMBARELLA_OSDRV_S3L_IAV_OFFSETS); do \
		name=$${want%%:*}; \
		got=$$(sed -n "s/^->$$name [#$$]*\([0-9]*\) .*/\1/p" $(@D)/abi/abi_probe.s); \
		if [ "$$got" != "$${want#*:}" ]; then \
			echo "ambarella-osdrv-s3l: $$name is '$$got' in this kernel," \
				"iav.ko needs $${want#*:}: check the kernel config" >&2; \
			exit 1; \
		fi; \
	done
endef

define AMBARELLA_OSDRV_S3L_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/sensor/config/*.cfg

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.104/ambarella
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.104/ambarella $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/firmware/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/load_ucode

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(AMBARELLA_OSDRV_S3L_PKGDIR)/files/lib/xxx.so
endef

$(eval $(generic-package))
