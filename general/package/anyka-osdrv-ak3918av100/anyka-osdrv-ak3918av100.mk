################################################################################
#
# anyka-osdrv-ak3918av100
#
################################################################################

ANYKA_OSDRV_AK3918AV100_VERSION =
ANYKA_OSDRV_AK3918AV100_SITE =
ANYKA_OSDRV_AK3918AV100_LICENSE = PROPRIETARY

# Anyka builds its ISP, encoder, audio, SD and USB host drivers per machine
# and ships them only prebuilt, for the AnyCloud39AV100 SDK's kernel, 4.4.282
# (vermagic "4.4.282 mod_unload ARMv5 "), which OpenIPC/linux
# anyka-ak3918av100 is, unchanged in every structure they use.
#
# files/kmod/av100: the AK3918AV100 (N/P/X/XP, KM01N/KM01D) builds, from
# image/usr.sqsh4 /usr/modules of the AnyCloud39AV100 SDK V1.12,
# https://github.com/KD0000001/ak3918av100 commit
# ead8c0b8bcc1e7c1c155737968b08b15154d2054, unmodified (isp_drv_lib V0.2.43):
#   6d0c7b238d950eef83633c44e6dd06a9ec477310157ae9b7cf9a6b2397fd21d4  ak_eth.ko
#   e8213bb364a4a414b158ec99e8fa937c37007383684deda503a8264d38aa96f6  ak_hcd.ko
#   956bee1eaf437780dfbd21bd0c33da807558c3565f665c554f5e4c015bead611  ak_i2c.ko
#   f5857d980fcf706e7e535029ea71e6ad8a8a7fdef169c4b75911bf4425dce7b6  ak_ion.ko
#   c51d13bec63e9738a4c4b0d0231484aa3c584a1f03b037c7f1591f6dbe2a5ff8  ak_isp.ko
#   59e919019d65d70ee1833dc1811cef4b3b8a45a2b807450ec5c1d2a0e57de33f  ak_mci.ko
#   bbc82c06ebdce7f85932947a6be0a8519bf147d67b8302e23213549f3244cba1  ak_pcm.ko
#   92787aafccfc5eec60b4a33bc5f849d4f90ef409406523a6b0f651b7d6da3829  ak_venc_adapter.ko
#   5bbbeeccf9a3a5d381e3541fa2b1aa372799cd93e82087a6f9e45d6f04f0c8d2  ak_venc_bridge.ko
#   892b255f26edb7b733fbd1dbad87cd191c30e45282fdc9f3718f3cca0a5b5e79  sensor_gc2063.ko
#   2a71274df5db1e1bef4d8aa367baaeb645422259c977ef552ba4b071bc8a16fe  sensor_gc3003.ko
#   91127552919b8d514ef5a1df34de6815fe15226e7396a3f4f7a964ce5c82f4c7  sensor_gc4653.ko
#   30ed9809226612a37f580273543c4ea69d047dcde83bcf77716faa3fc9a7f9bb  sensor_mis2008.ko
#   20d66da281410bf6d6337eea044f5e76da1515e6bcbe7a2d196e630571021651  sensor_sc1346.ko
#   c0662cd584f0383c6f68eb1b18b2d579931ab4343998ddc4a37341a14c0945a7  sensor_sc223a.ko
#   0b1fbd060865e0fb26d907918b18a76d96722c448ae44bbb23221dae9357ba82  sensor_sc2336.ko
#   a60cc1e1c09d2123f4473de408647fd7a5639f729e6f21beca61e43423f7b8f8  sensor_sc2336x.ko
#   d549123091abf09fd722cb505f40938bf88449001d01700859217105e8eda475  sensor_sc3336.ko
#   604e31f2ece6a03c37f47033868680c07f0a6dd676887c7591b74a99af6573e8  sensor_sc401ai.ko
#   6f90b193427965f5d341ddf42508c91ae0571613db4c299584519d86fac32472  sensor_sc5238.ko
#
# files/kmod/ev300l: the AK3918EV300L builds. No AnyCloud39EV300L SDK is
# public, and the AV100 ones do not serve it: its ak_isp drives the AV100's
# second MIPI input, and on an EV300L device tree fails without it
# (mipi1_gclk). These come from the stock firmware of a GZhongshi HVT3610S3
# camera (vendor U-Boot 2019.10.0-V4.0.25, kernel 4.4.282 of 2024-04-10),
# /usr/modules and /mvs/modules, unmodified (isp_drv_lib V0.2.30):
#   aaa8a9ae27e7427d9d6f7a4570bedf93ad9eef52699b1edd75ebdf16782335b1  ak_hcd.ko
#   5928207d9fdbb44a8d3788608c2fbaf3daaeda2d86710c61084091e4ff177797  ak_eth.ko
#   8f19dbbcc8e82254308dae822e9ea5e5b85988535e0dad96c99c30f5e8c9dd53  ak_i2c.ko
#   9bad58fa412cf42b612caca95b8f223fcff397b3a5b743cab8f992172b1ec87b  ak_ion.ko
#   07f871f2bfe74c9a8ce71dd999788b7570987edb0dae7097df7f654251d7c181  ak_isp.ko
#   4bc5d2ab2d806fc5ed55550245b14e910c2db1798b879c449dd6b1019163c827  ak_mci.ko
#   b5f7b91e9f0c25a9b77579ac6c2fcff4930971815f965d0a6ee8bb2162a5dee9  ak_pcm.ko
#   f510acfa81af03c834dbcffe6a90622ad74f3350e9f5eaf84f0a2ec065c39306  ak_venc_adapter.ko
#   4941fb8e4db7837df26f41c49cf8b96c03d9e567371b902f600a9b6225f8d3ce  ak_venc_bridge.ko
#   a77faa572d5f72a087806199aca5626d7901825c4958479581e4ebdea6834932  sensor_gc3003.ko
#   78ea187589b0f9c7ca2a3c0e9496ac3be1c20e9724bab4b26402bd557e93e359  sensor_mis2006.ko
#   7fde778cafaf22dd5582298ad5b18bc7cdde01b85d76bdf2b87dd5a09f00e04c  sensor_mis2008.ko
#   bf2ddb88224e367ad30b9d42cf4a136986cf90d86d44f2b1d4a82d76040d186d  sensor_sc1346.ko
#   4e45107ef7bbc542b14cec9363af6f3dce1eb95a893ff0ac423996292b9f2a0b  sensor_sc2331.ko
#   e2ee28337e8ffda436dd840d4b79f137b1e0e0c8fdbcbba1857c9f9457372afc  sensor_sc2332.ko
#   ff797966915aeab96e458186b0967a7851b6a26c85e0eeea6d7d001fcfa9e5b3  sensor_sc2336.ko
#
# Checked against OpenIPC/linux anyka-ak3918av100 on qemu-hisilicon's
# ak3918ev300l machine: the EV300L set loads and its ISP, encoder, bridge
# and audio drivers probe (/dev/video-0-*, /dev/venc-chn*, /dev/pcmC0D0*);
# the AV100 set's ak_i2c, ak_pcm, ak_ion and ak_venc_adapter do too, and its
# ak_isp is for the AV100. None of them has run on a camera under OpenIPC
# yet: they have booted only in qemu-bench, and the image is published so
# that owners of these cameras can be the first to run it.
#
# files/lib/av100: the AnyCloud39AV100 SDK V1.12 libraries majestic drives these
# modules with (libplat_vi, libplat_vpss, libmpi_venc and the libplat_* they
# call), from the same image/usr.sqsh4, /usr/lib. They are uClibc builds;
# like ingenic-osdrv-t41's, their DT_NEEDED libc.so.0 is dropped and they are
# stripped, nothing else. sha256 of the SDK's files:
#   faa04949482fa55d52f9f32ad16170022301655d5dd25d6ae938ee161771fe0b  libmpi_venc.so
#   ecdc634da07c9087a3817f20ec01fadbe7d180cf2ec620725f5a94ac019c4f17  libplat_common.so
#   89b67330127100248e12d1bcd16525dcd4c2c643146b3dd00c2e91913dfd890b  libplat_dbg.so
#   ed7c7577fb83fa0db4f4903fdca8a68eeb7a8bfd6a38e4375295ccacd2577248  libplat_isp_sdk.so
#   01eb68eba1f6e5ff8dab5a56faec8885397d18b935698c2a17be7250fb4830a2  libplat_log.so
#   90f5673b0b4f0f3e15ab18a8d3b492d6fa65b27492d462954b9cdf87744ac38f  libplat_mem.so
#   2a433e2292fb32f62093960655d7579da7148020be2e3428e1a833ee98cfd81f  libplat_osal.so
#   47b127752384f07e80cfcc3afdf1cfe4a39713c3b8ec2508e5acca2eca0fb438  libplat_thread.so
#   a88b8ed307b7d72e55fe480479e643ce606594735729a6bb09036ab28ab58b83  libplat_vi.so
#   0e41057caf20e9fd1c11b741a1720d1a677581d45dfd14729549f1237f7f74a6  libplat_vpss.so
# files/sensor/av100: that image's /etc ISP tuning for the single-sensor boards
# whose drivers files/kmod/av100 carries, unmodified:
#   77c0a40549b7edca0f506b35ba7d03a56e85ab44b1dd39b33cc4c07620bd528b  isp_gc3003_mipi_2lane_av100_hp.conf
#   4d0b175f9abb80e3798ab7a4bf4a147c842af62aff5550bc337377dc8b85a8fd  isp_gc4653_mipi_2lane_av100_hp.conf
#   584712be6ca5e83991d0c21424f14828b0c009c2c02cdfbd6632d6fa5166600f  isp_sc2336_mipi_2lane_av100.conf
#   bb2b7ee2f0fdc6d78f1d9141d607c21a46e2cca262926e7ae34ec298a91250c1  isp_sc5238_mipi_2lane_av100_hp.conf
# files/lib/ev300l: the libraries the EV300L set's camera firmware drives
# those modules with. They are not the SDK's: libplat_vi V3.4.17,
# libplat_vpss V2.1.09, libmpi_venc V2.0.40 and libplat_common V1.0.23, where
# the SDK has V3.4.23, V2.1.17, V2.0.46 and V1.0.25. They export every symbol
# majestic.ak3918av100 calls. They come from the /usr of an EV300L camera
# (EVB_CBDM_AK3918EV300L_V1.0.0, two SC1346), whose /usr/modules are
# byte-identical to files/kmod/ev300l. Treated as files/lib/av100's are, with
# DT_NEEDED libc.so.0 dropped; they ship stripped. sha256 of the camera's files:
#   890f4e54f1b4c49cf24521576c355d2c5508f5f2f28cbd6f88f929f80dcca6e3  libmpi_venc.so
#   593304e69493e465fab3abe79a3ca1b0ddd4d2037b0245e11d12673ed8764d45  libplat_common.so
#   6974fde4bb1426b589432165c7775e3173dd00ceb10dd1ffcf55a803e7d50a6d  libplat_dbg.so
#   557687bcc624e242350d05f353fca59d843b919bb7580a106e623edac5b4773c  libplat_isp_sdk.so
#   3066bb105ee4cc8b58904a71dca735b99f5ce6b3a1cd2d79863925de3f774aab  libplat_log.so
#   90f5673b0b4f0f3e15ab18a8d3b492d6fa65b27492d462954b9cdf87744ac38f  libplat_mem.so
#   75018adf596948b694bf9d33f010ea347d6dd8977da18443600e29ddbf00c02f  libplat_osal.so
#   47b127752384f07e80cfcc3afdf1cfe4a39713c3b8ec2508e5acca2eca0fb438  libplat_thread.so
#   7027af4e3be00cacb5a664350f0ff75c3aa2177b5b308b14f736c57c1b8bf423  libplat_vi.so
#   516777f7ae6fd9fd876c9dc3fcda2102cd2f7179063485b34d01b49b73e32275  libplat_vpss.so
# files/sensor/ev300l: the same camera's /mvs/isp_cfg tuning, unmodified.
# The SC1346 has one file per MIPI input, and the two differ:
#   ecf3ed63114104fbf6b3a18707c8703af877f805b6d4b8ec4eaad3c130daf824  isp_sc1346_mipi_0.conf
#   35fbff49096ecee68ee579493ae0b86caa51ab6b845044958a1798d97c5e8856  isp_sc1346_mipi_1.conf
#   b89bcf6933efcd4946789b9d68421712e64129f6c2baf56f324b6a5bbc52fb6e  isp_sc2331_mipi_0.conf
#   2d1a51948cae94639791fcf82a272714e0fc6c15be75c8e988f5b96815f376e6  isp_sc2336_mipi_0.conf
# majestic tries each until the library takes one; another sensor's is
# refused. The libraries and the tuning serve majestic alone, so they are
# installed only beside it.
ANYKA_OSDRV_AK3918AV100_KVER = 4.4.282

ifeq ($(BR2_OPENIPC_SOC_MODEL),"ak3918ev300l")
ANYKA_OSDRV_AK3918AV100_KMOD = ev300l
else
ANYKA_OSDRV_AK3918AV100_KMOD = av100
endif

ANYKA_OSDRV_AK3918AV100_DEPENDENCIES = linux

# Installed under the release the built kernel reports, and the build stops
# when it is not the one these were built for: they would not load.
define ANYKA_OSDRV_AK3918AV100_INSTALL_TARGET_CMDS
	kver=$(LINUX_VERSION_PROBED); \
	if [ "$$kver" != "$(ANYKA_OSDRV_AK3918AV100_KVER)" ]; then \
		echo "anyka-osdrv-ak3918av100: kernel release is '$$kver'," \
			"these modules were built for $(ANYKA_OSDRV_AK3918AV100_KVER)" >&2; \
		exit 1; \
	fi; \
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/$$kver/anyka && \
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$$kver/anyka \
		$(ANYKA_OSDRV_AK3918AV100_PKGDIR)/files/kmod/$(ANYKA_OSDRV_AK3918AV100_KMOD)/*.ko

	if [ -d $(ANYKA_OSDRV_AK3918AV100_PKGDIR)/files/modprobe/$(ANYKA_OSDRV_AK3918AV100_KMOD) ]; then \
		$(INSTALL) -D -m 644 \
			$(ANYKA_OSDRV_AK3918AV100_PKGDIR)/files/modprobe/$(ANYKA_OSDRV_AK3918AV100_KMOD)/anyka.conf \
			$(TARGET_DIR)/etc/modprobe.d/anyka.conf; \
	fi

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(ANYKA_OSDRV_AK3918AV100_PKGDIR)/files/script/load_anyka
	$(ANYKA_OSDRV_AK3918AV100_INSTALL_MAJESTIC_SDK)
endef

ifeq ($(BR2_PACKAGE_MAJESTIC),y)
define ANYKA_OSDRV_AK3918AV100_INSTALL_MAJESTIC_SDK
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(ANYKA_OSDRV_AK3918AV100_PKGDIR)/files/lib/$(ANYKA_OSDRV_AK3918AV100_KMOD)/*.so
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor $(ANYKA_OSDRV_AK3918AV100_PKGDIR)/files/sensor/$(ANYKA_OSDRV_AK3918AV100_KMOD)/*.conf
endef
endif

$(eval $(generic-package))
