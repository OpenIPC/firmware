################################################################################
#
# hisilicon-osdrv-hi3516cv6xx
#
# Load script + vendor userspace MPP libraries for the Hi3516CV6xx (V5)
# family. Kernel modules and sensor drivers are no longer shipped here:
#  * kernel modules now built against the running openipc/linux tree by
#    hisilicon-opensdk (see ../hisilicon-opensdk/hisilicon-opensdk.mk).
#    Shipping prebuilt vendor .ko caused init-time hangs because struct
#    module / vermagic / config layouts didn't match openipc's kernel.
#  * sensor drivers now built from vendor SDK source by hisilicon-opensdk
#    via libraries/sensor/hi3516cv6xx/ and HISILICON_OPENSDK_SENSORS_hi3516cv6xx.
#
# Vendor MPP userspace .so blobs ship from files/lib/ — no openhisilicon
# V5 source mirror exists. The install list below is the transitive
# NEEDED closure of the majestic binary against the vendor lib set
# (computed via readelf -d), with libopus.so excluded because
# BR2_PACKAGE_OPUS_OPENIPC builds a source equivalent. The 13 libs not
# in the closure (libss_mpi_aibnr/bla/cipher/devstat/km/otp/smartae/
# syskol/uvc, libss_bcd, libmbedtls_harden_adapt, libsvp_aicpu,
# libvqe_common) save ~660 KB and are intentionally omitted.
#
################################################################################

HISILICON_OSDRV_HI3516CV6XX_VERSION =
HISILICON_OSDRV_HI3516CV6XX_SITE =
HISILICON_OSDRV_HI3516CV6XX_LICENSE = MIT
HISILICON_OSDRV_HI3516CV6XX_LICENSE_FILES = LICENSE

# Vendor MPP userspace .so files needed by majestic (transitive NEEDED
# closure). Excludes libopus.so (provided by BR2_PACKAGE_OPUS_OPENIPC).
HISILICON_OSDRV_HI3516CV6XX_MPP_LIBS = \
	libaac_comm.so libaac_dec.so libaac_enc.so \
	libaac_sbr_dec.so libaac_sbr_enc.so \
	libacs.so libaiisp.so libbnr.so libcalcflicker.so \
	libdehaze.so libdnvqe.so libdrc.so \
	libextend_stats.so libir_auto.so libldci.so \
	libmp3_dec.so libmp3_enc.so libmp3_lame.so \
	libot_mpi_isp.so libot_osal.so libsecurec.so \
	libss_ivs_md.so \
	libss_mpi.so libss_mpi_ae.so libss_mpi_audio.so libss_mpi_audio_adp.so \
	libss_mpi_awb.so libss_mpi_isp.so libss_mpi_ive.so \
	libss_mpi_sysbind.so libss_mpi_sysmem.so \
	libsvp_acl.so libupvqe.so libvoice_engine.so \
	libvqe_aec.so libvqe_agc.so libvqe_anr.so libvqe_eq.so \
	libvqe_hpf.so libvqe_hs.so libvqe_record.so libvqe_res.so libvqe_talkv2.so

define HISILICON_OSDRV_HI3516CV6XX_INSTALL_TARGET_CMDS

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(foreach lib,$(HISILICON_OSDRV_HI3516CV6XX_MPP_LIBS), \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/lib/$(lib) ; \
	)

endef

$(eval $(generic-package))
