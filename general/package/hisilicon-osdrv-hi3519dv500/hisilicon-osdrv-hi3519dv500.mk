################################################################################
#
# hisilicon-osdrv-hi3519dv500
#
# Load script + vendor userspace MPP libraries for the Hi3519DV500 / Hi3516DV500
# (V5 "HISI_OT", aarch64 Cortex-A55) family. Mirrors hisilicon-osdrv-hi3516cv6xx.
#
# Vendor MPP userspace .so ship from files/lib/, extracted from the Hi3519DV500
# SDK V1.0.1.1 (HI3519DV500_MPP_V1.0.1.1 B040) out/lib/. The list is the
# hi3516cv6xx MPP set intersected with what the DV500 SDK provides (cv6xx's
# libot_osal / libvqe_hs are absent on DV500); libopus.so is excluded
# (BR2_PACKAGE_OPUS_OPENIPC + the HiSi shim provide an open equivalent).
#
################################################################################

HISILICON_OSDRV_HI3519DV500_VERSION =
HISILICON_OSDRV_HI3519DV500_SITE =
HISILICON_OSDRV_HI3519DV500_LICENSE = MIT
HISILICON_OSDRV_HI3519DV500_LICENSE_FILES = LICENSE

# Vendor MPP userspace .so files (hi3516cv6xx set ∩ DV500 SDK out/lib).
HISILICON_OSDRV_HI3519DV500_MPP_LIBS = \
	libaac_comm.so libaac_dec.so libaac_enc.so \
	libaac_sbr_dec.so libaac_sbr_enc.so \
	libacs.so libaiisp.so libbnr.so libcalcflicker.so \
	libdehaze.so libdnvqe.so libdrc.so \
	libextend_stats.so libir_auto.so libldci.so \
	libmp3_dec.so libmp3_enc.so libmp3_lame.so \
	libot_mpi_isp.so libsecurec.so \
	libss_ivs_md.so \
	libss_mpi.so libss_mpi_ae.so libss_mpi_audio.so libss_mpi_audio_adp.so \
	libss_mpi_awb.so libss_mpi_isp.so libss_mpi_ive.so \
	libss_mpi_sysbind.so libss_mpi_sysmem.so \
	libsvp_acl.so libupvqe.so libvoice_engine.so \
	libvqe_aec.so libvqe_agc.so libvqe_anr.so libvqe_eq.so \
	libvqe_hpf.so libvqe_record.so libvqe_res.so libvqe_talkv2.so

define HISILICON_OSDRV_HI3519DV500_INSTALL_TARGET_CMDS

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3519DV500_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3519DV500_PKGDIR)/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(foreach lib,$(HISILICON_OSDRV_HI3519DV500_MPP_LIBS), \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3519DV500_PKGDIR)/files/lib/$(lib) ; \
	)

endef

$(eval $(generic-package))
