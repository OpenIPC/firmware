################################################################################
#
# hisilicon-osdrv-hi3516ev200
#
################################################################################

HISILICON_OSDRV_HI3516EV200_VERSION =
HISILICON_OSDRV_HI3516EV200_SITE =
HISILICON_OSDRV_HI3516EV200_LICENSE = MIT
HISILICON_OSDRV_HI3516EV200_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3516EV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/script/S95hisilicon

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/WDR
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/WDR $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/sensor/config/WDR/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/sensor/iq/imx307.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/sensor/iq/imx335.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/sensor/iq/f23.ini
	ln -sf imx307.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/kmod/camhi-motor.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/kmod/hi3516ev200_isp.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/script/load_hisilicon
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libaaccomm.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libaacdec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libaacenc.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libaacsbrdec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libaacsbrenc.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libbcd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/lib_hiae.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/lib_hiawb_natura.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/lib_hiawb.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhi_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/lib_hidehaze.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/lib_hidrc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/lib_hiir_auto.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/lib_hildci.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhive_AEC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhive_AGC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhive_ANR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhive_common.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhive_EQ.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhive_HPF.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhive_record.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libhive_RES.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libive.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libivp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libsecurec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516ev200/files/lib/libVoiceEngine.so
endef

$(eval $(generic-package))
