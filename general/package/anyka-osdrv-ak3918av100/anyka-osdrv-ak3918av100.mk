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
# ak_isp is for the AV100. None has run on a camera under OpenIPC yet, which
# keeps these boards out of ALL_BOARDS.
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
endef

$(eval $(generic-package))
