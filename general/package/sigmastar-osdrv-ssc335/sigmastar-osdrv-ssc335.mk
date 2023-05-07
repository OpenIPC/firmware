################################################################################
#
# sigmastar-osdrv-ssc335
#
################################################################################

SIGMASTAR_OSDRV_SSC335_VERSION =
SIGMASTAR_OSDRV_SSC335_SITE =
SIGMASTAR_OSDRV_SSC335_LICENSE = MIT
SIGMASTAR_OSDRV_SSC335_LICENSE_FILES = LICENSE

define SIGMASTAR_OSDRV_SSC335_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iqfile
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/sensor/iqfile/filter.txt
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/sensor/iqfile/imx307_iqfile.bin
	ln -sf imx307_iqfile.bin $(TARGET_DIR)/etc/sensors/iqfile/iqfile0.bin
	ln -sf imx307_iqfile.bin $(TARGET_DIR)/etc/sensors/iqfile/iqfile1.bin
	ln -sf imx307_iqfile.bin $(TARGET_DIR)/etc/sensors/iqfile/iqfile2.bin
	ln -sf imx307_iqfile.bin $(TARGET_DIR)/etc/sensors/iqfile/iqfile3.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/venc_fw
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/venc_fw $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/sensor/venc_fw/chagall.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.84/sigmastar
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/*.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mhal.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_common.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_rgn.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_divp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_ipu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_vpe.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_sensor.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_vif.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/kmod/mi_shadow.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.84/sigmastar $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/sensor/sensor_*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/script/debug_level

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# Minimalism for sample_venc_jpeg
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_sys.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_sensor.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_vif.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_vpe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_isp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libcus3a.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libispalgo.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_venc.so
	# Maximum for demo software
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_rgn.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_divp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_iqserver.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_vdf.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_shadow.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libOD_LINUX.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libMD_LINUX.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libVG_LINUX.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_ive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_ao.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libcam_fs_wrapper.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libcam_os_wrapper.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(SIGMASTAR_OSDRV_SSC335_PKGDIR)/files/lib/libmi_common.so
endef

$(eval $(generic-package))
