################################################################################
#
# goke-osdrv-gk7205v500
#
################################################################################

GOKE_OSDRV_GK7205V500_VERSION =
GOKE_OSDRV_GK7205V500_SITE =
GOKE_OSDRV_GK7205V500_LICENSE = MIT
GOKE_OSDRV_GK7205V500_LICENSE_FILES = LICENSE
GOKE_OSDRV_GK7205V500_INSTALL_STAGING = YES

# The V500 SDK has two halves. The implementation is libxmedia_*.so, which
# export MPI_* / XMEDIA_API_*; beside them the vendor ships libhi_*.so, thin
# shims that re-export the HiSilicon-spelled HI_MPI_* names and forward each
# one to its XMEDIA_API_* twin. majestic links against the shimmed half --
# its FindHiSiliconSDK gives SDK code 7205500 the same hi_mpi/hi_md/hi_isp/
# hi_ive/hi_ae/hi_awb list as the GK7205V200 -- so without these six, and
# without libvqe.so that libxmedia_api.so needs for its audio_vqe_* calls,
# musl refuses to start it at all ("Error loading shared library
# libhi_mpi.so", then ~250 HI_MPI_* relocation failures) and the camera
# streams nothing. Reported on a GK7202V500 in #2428; goke-osdrv-gk7205v200
# has shipped its own libhi_* shims since the family was added.
#
# Only a majestic image can use them, so the three lite boards that share this
# package do not pay the 32KB.
ifeq ($(BR2_PACKAGE_MAJESTIC),y)
define GOKE_OSDRV_GK7205V500_INSTALL_MAJESTIC_LIBS
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_ae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_awb.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_isp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_ive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_md.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_mpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe.so
endef
endif

define GOKE_OSDRV_GK7205V500_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/iq/sc2232.ini
	ln -sf sc2232.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/goke
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_acodec.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_adc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_chnl.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_cipher_drv.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_gfbg.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_h265e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_isp.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_isp_piris.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_isp_pwm.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_isp_sample_ist.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_isp_sensor_i2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_isp_sensor_spi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_jpege.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_mipi_rx.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_osal.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_pdm.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_rgn.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_ssp_ota5182_ex.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_ssp_st7789_ex.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_ssp_st7796_ex.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_sysconfig.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_tde.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_vedu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_vi.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_vo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_vpss.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod/xm_wdt.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_ar0237.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_f37p.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_f37.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc2053_forcar.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc2053.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc2083.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc2093.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc3023.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc4023.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc4653_2l.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc4663.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_gc5603.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_imx290.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_imx307_2l.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_imx307.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_imx327_2l.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_imx327.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_imx335.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_k03.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_k04.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_k05.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_k06.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_k17.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_k303.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_os02g10.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_os03b10.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_os04b10.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_os04c10_2l.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_os04c10.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_os05a_2l.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_os05a.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_ov2718.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_q03p.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_q03.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc2231.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc2235.so
	# SC223A is the die behind the SC5239S these boards are sold with, and the
	# SC2239P and SC233A labels too -- one part, several names, which is why the
	# sensor the seller advertises is not the one the driver is called after.
	# sc223a_i2c_1080p.ini already ships and names this driver, so only the half
	# majestic dlopens from /usr/lib/sensors was missing and a camera with this
	# sensor had no video however it was configured. The four-lane
	# 4l_sc223a_i2c_1080p.ini beside it is NOT served by this: it names
	# libsns_sc223a_4l.so, which the V500 SDK does not carry in any flavour.
	# It is one of six configs here naming a driver this package does not have
	# (the others are jxf23, jxf23_dc, mis2008, sc200ai, sc2232h), inherited
	# with the config set from a family whose SDK did. #2428.
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc223a.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc2336.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc3235.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc3335.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc3336.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc401ai.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc4236.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc4336.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc500ai.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc530ai_2l.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc530ai.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc5336.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc8238.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sp2305.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/*
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libaac_comm.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libaac_dec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libaac_enc.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libaac_sbr_dec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libaac_sbr_enc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libawb.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libbcd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libdehaze.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libdrc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libir_auto.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libldci.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libmp3_dec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libmp3_enc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libsecurec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvoice_engine.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_3a.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_common.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_eq.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_gain.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_hpf.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_res.so
	# libvqe.so is installed by GOKE_OSDRV_GK7205V500_INSTALL_MAJESTIC_LIBS above.
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_talkv2.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_wnr.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_ae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_api.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_awb_natura.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_awb.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_isp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_ive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_ivp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_md.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_qr.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libxmedia_tde.so

	$(GOKE_OSDRV_GK7205V500_INSTALL_MAJESTIC_LIBS)
endef

$(eval $(generic-package))
