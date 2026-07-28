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
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/libsns_sc223a.so
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
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe.so
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

endef

# ---------------------------------------------------------------------------
# GK7201V200 (XiongMai xm72010200)
#
# This die is served by the gk7205v500 family but not by its module set. The stock
# Apr-2023 xm_*.ko above bring it up as far as the ISP and then stop: VEDU CreateChn
# fails with F008FFFF and the VI->VPSS online link never starts, so there is no
# picture at all. The camera's own factory firmware carries a Sep-2023 build of the
# same vendor MPP, for chip id xm72010200, which drives this silicon correctly - same
# vermagic, so it loads on the same kernel. Those modules are in
# files/kmod.gk7201v200/; see the PROVENANCE.md there for where they came from and how
# to re-extract them.
#
# Everything below is confined to this hook and to BR2_OPENIPC_SOC_MODEL, so
# gk7205v500_lite, gk7205v500_ultimate and gk7205v510_lite are byte-for-byte unaffected.
# The Sep-2023 modules against the Apr-2023 userspace above is a real API skew; the
# gk7201-fixup package bridges it.
ifeq ($(BR2_OPENIPC_SOC_MODEL),"gk7201v200")
define GOKE_OSDRV_GK7205V500_GK7201V200_INSTALL
	# Exactly the modules load_xm insmods, and no others - the directory holds the whole
	# vendor set, but shipping the unused half would cost 144 KB of a 8 MB rootfs.
	# Two of the stock list are deliberately dropped by the rm above: xm_wdt.ko, which
	# hardcodes the wrong interrupt on this kernel and would reset the board every 60 s
	# (the SP805 driver handles the watchdog instead, see kernel patch 0902), and
	# xm_tde.ko, which nothing here loads. xm_isp_pwm.ko is not shipped either: load_xm
	# tries it with "|| true" for boards with a DC iris, and this one has none, so it has
	# never been exercised.
	rm -f $(TARGET_DIR)/lib/modules/4.9.37/goke/xm_*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_acodec.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_adc.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_adec.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_aenc.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_ai.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_aio.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_ao.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_base.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_chnl.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_h264e.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_h265e.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_isp.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_isp_sensor_i2c.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_isp_sensor_spi.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_ive.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_jpege.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_mipi_rx.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_osal.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_pdm.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_pm.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_rc.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_rgn.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_sysconfig.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_sys.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_vedu.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_venc.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_vgs.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_vi.ko \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/xm_vpss.ko

	# sinit.ko: the vendor's sensor-init helper, loaded between the two MCLK pokes in
	# load_xm. Nothing else in this package has an extdrv/ directory.
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/goke/extdrv
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke/extdrv \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/kmod.gk7201v200/extdrv/sinit.ko

	# Audio VQE. libxmedia_api.so resolves these at runtime; without them the audio
	# path in the Sep-2023 stack does not come up. Left out of the list above because
	# no other board in this family needs them.
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_3a.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_common.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_eq.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_gain.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_hpf.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_res.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_talkv2.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libvqe_wnr.so

	# libhi_* facades: Majestic links the HiSilicon MPI names, this vendor stack exports
	# the xmedia ones. Six thin forwarding libraries close that gap; see
	# gen_xmedia_shim.sh for how they are generated.
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_mpi.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_ae.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_awb.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_isp.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_ive.so \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/lib/libhi_md.so

	# SC2336P IQ data and the IR-cut helper for this board.
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/sensor/iq/sc2336p.bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin \
		$(GOKE_OSDRV_GK7205V500_PKGDIR)/files/script/ircut_gk7201v200
endef
GOKE_OSDRV_GK7205V500_POST_INSTALL_TARGET_HOOKS += GOKE_OSDRV_GK7205V500_GK7201V200_INSTALL
endif

$(eval $(generic-package))
