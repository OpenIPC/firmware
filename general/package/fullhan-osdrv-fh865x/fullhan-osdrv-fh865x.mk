################################################################################
#
# fullhan-osdrv-fh865x
#
################################################################################

FULLHAN_OSDRV_FH865X_VERSION =
FULLHAN_OSDRV_FH865X_SITE =
FULLHAN_OSDRV_FH865X_LICENSE = MIT
FULLHAN_OSDRV_FH865X_LICENSE_FILES = LICENSE
FULLHAN_OSDRV_FH865X_DEPENDENCIES = linux

# FH865x runs the FH885xV200 media code (its media_process carries
# chip_fh865x) on a Cortex-A7, so the SDK's prebuilt ARMv6 modules do not
# load; the SDK ships them as objects to link against the board's kernel
# (media_support/make_drivers), and that is done here. Linked against the
# FH885xV200 kernel, these objects reproduce fullhan-osdrv-fh8852v200's
# modules exactly. Everything userspace is that package's.
FULLHAN_OSDRV_FH865X_V200 = $(BR2_EXTERNAL_GENERAL_PATH)/package/fullhan-osdrv-fh8852v200/files

FULLHAN_OSDRV_FH865X_MODULES = xbus_rpc vmm media_process media_process_no_sc \
	vpu isp enc jpeg

# isp, vpu, enc and jpeg take their symbols from media_process, which
# takes its from xbus_rpc
define FULLHAN_OSDRV_FH865X_BUILD_CMDS
	cp -r $(FULLHAN_OSDRV_FH865X_PKGDIR)/files/make_drivers $(@D)/
	$(foreach m,$(FULLHAN_OSDRV_FH865X_MODULES),
		$(LINUX_MAKE_ENV) $(MAKE) -C $(LINUX_DIR) $(LINUX_MAKE_FLAGS) \
			M=$(@D)/make_drivers/$(m) \
			KBUILD_EXTRA_SYMBOLS="$(@D)/make_drivers/xbus_rpc/Module.symvers \
				$(if $(filter vpu isp enc jpeg,$(m)),$(@D)/make_drivers/media_process/Module.symvers)" \
			modules$(sep))
endef

define FULLHAN_OSDRV_FH865X_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.129/fullhan
	$(foreach m,$(FULLHAN_OSDRV_FH865X_MODULES),
		$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.129/fullhan \
			$(@D)/make_drivers/$(m)/$(m).ko$(sep))

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/firmware
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/firmware $(FULLHAN_OSDRV_FH865X_V200)/firmware/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(FULLHAN_OSDRV_FH865X_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(FULLHAN_OSDRV_FH865X_V200)/lib/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(FULLHAN_OSDRV_FH865X_V200)/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(FULLHAN_OSDRV_FH865X_V200)/sensor/params/*
endef

$(eval $(generic-package))
