################################################################################
#
# t23-ipa
#
################################################################################

T23_IPA_SITE_METHOD = local
T23_IPA_SITE = $(T23_IPA_PKGDIR)/src
T23_IPA_LICENSE = GPL-2.0-or-later
T23_IPA_LICENSE_FILES = ipa_core.c
T23_IPA_DEPENDENCIES = linux

T23_IPA_SRCS = ipa_core.c ipa_tuning.c ipa_sensor.c ipa_ae.c ipa_awb.c \
	ipa_color.c ipa_daynight.c ipa_dns.c

# Install the 6.12 kernel's sanitized UAPI headers (media/ingenic/t23-isp-config.h,
# media.h, videodev2.h, v4l2-subdev.h, gpio.h) because the external
# toolchain ships 3.10 headers.
define T23_IPA_BUILD_CMDS
	$(HOST_MAKE_ENV) $(MAKE) -C $(LINUX_DIR) ARCH=mips \
		headers_install INSTALL_HDR_PATH=$(@D)/uapi
	$(TARGET_CC) $(TARGET_CFLAGS) -std=gnu99 -D_GNU_SOURCE \
		-isystem $(@D)/uapi/include \
		$(addprefix $(@D)/,$(T23_IPA_SRCS)) \
		$(TARGET_LDFLAGS) -lm -o $(@D)/t23-ipa
endef

define T23_IPA_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/t23-ipa $(TARGET_DIR)/usr/sbin/t23-ipa
	$(INSTALL) -D -m 644 $(T23_IPA_PKGDIR)/files/t23-ipa.default \
		$(TARGET_DIR)/etc/default/t23-ipa
	$(INSTALL) -D -m 644 -t $(TARGET_DIR)/etc/sensor \
		$(T23_IPA_PKGDIR)/files/tuning/*.yaml
	ln -sf sc2336-t23.yaml $(TARGET_DIR)/etc/sensor/sc2336p-t23.yaml
	ln -sf sc2336-t23.yaml $(TARGET_DIR)/etc/sensor/sc2337p-t23.yaml
endef

define T23_IPA_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 755 $(T23_IPA_PKGDIR)/files/S80t23-ipa \
		$(TARGET_DIR)/etc/init.d/S80t23-ipa
endef

$(eval $(generic-package))
