################################################################################
#
# novatek-osdrv-nt9856x
#
################################################################################

NOVATEK_OSDRV_NT9856X_VERSION =
NOVATEK_OSDRV_NT9856X_SITE =
NOVATEK_OSDRV_NT9856X_LICENSE = MIT
NOVATEK_OSDRV_NT9856X_LICENSE_FILES = LICENSE

define NOVATEK_OSDRV_NT9856X_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/script/S95novatek

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/sensor/config/sen_sc*.cfg

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/profiles
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/profiles $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/sensor/profiles/isp*.cfg

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kwrap.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/comm/nvtmem
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/comm/nvtmem $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/comm/nvtmem/nvtmem.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_builtin
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_builtin $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_builtin/kdrv_builtin.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/comm
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/comm $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/comm/kdrv_comm.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_gfx2d
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_gfx2d $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_gfx2d/kdrv_gfx2d.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_videocapture
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_videocapture $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_videocapture/kdrv_videocapture.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_videoprocess
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_videoprocess $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_videoprocess/kdrv_videoprocess.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_common
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_common $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_common/kflow_common.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_gfx/videosprite
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_gfx/videosprite $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_gfx/videosprite/nvt_videosprite.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_videocapture
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_videocapture $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_videocapture/kflow_videocapture.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_videoprocess
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_videoprocess $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_videoprocess/kflow_videoprocess.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_gfx
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_gfx $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_gfx/nvt_gfx.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_videocodec
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_videocodec $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_videocodec/kdrv_h26x.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/nvt_vencrc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/nvt_vencrc $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/nvt_vencrc/nvt_vencrc.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_videoenc/unit
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_videoenc/unit $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_videoenc/unit/kflow_videoenc.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_audioio/audio_common
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_audioio/audio_common $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_audioio/audio_common/nvt_audio.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_audioio/kdrv_audio
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_audioio/kdrv_audio $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_audioio/kdrv_audio/nvt_kdrv_audio.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_audiocapture/unit
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_audiocapture/unit $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_audiocapture/unit/kflow_audiocap.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_audioout/unit
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_audioout/unit $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_audioout/unit/kflow_audioout.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_audioio/audlib_aac
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_audioio/audlib_aac $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_audioio/audlib_aac/nvt_audlib_aac.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_audioio/audlib_g711
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kdrv_audioio/audlib_g711 $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kdrv_audioio/audlib_g711/nvt_audlib_g711.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_audioenc/unit
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_audioenc/unit $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_audioenc/unit/kflow_audioenc.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_audiodec/unit
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/kflow_audiodec/unit $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/kflow_audiodec/unit/kflow_audiodec.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/isp
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/isp $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/isp/nvt_isp.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/ae
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/ae $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/ae/nvt_ae.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/awb
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/awb $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/awb/nvt_awb.ko
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek/hdal/iq $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/kmod/hdal/iq/nvt_iq.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/script/load*
	#$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/script/ircut_demo

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/sensor/sen_sc401ai/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/sensor/sen_sc500ai/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.19.91/novatek $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/sensor/sen_sc501ai/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/lib/libhdal.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/lib/libvendor_isp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_NOVATEK_PATH)/package/novatek-osdrv-nt9856x/files/lib/libvos.so
endef

$(eval $(generic-package))
