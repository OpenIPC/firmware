################################################################################
#
# divinus
#
################################################################################

DIVINUS_SITE = $(call github,openipc,divinus,$(DIVINUS_VERSION))
DIVINUS_VERSION = HEAD
DIVINUS_LICENSE = MIT
DIVINUS_LICENSE_FILES = LICENSE

ifeq ($(BR2_TOOLCHAIN_USES_GLIBC),y)
	DIVINUS_OPTIONS = "-rdynamic -s -Os -lm"
else ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_MSC313E),y)
	# The infinity3 SDK libraries are glibc builds: link the shim ahead of musl
	# (see glibc-compat/Config.in). Their threads also expect glibc's large
	# default stack; musl's 128 KiB one overflows in the camera pipeline.
	DIVINUS_DEPENDENCIES += glibc-compat
	DIVINUS_OPTIONS = "-rdynamic -s -Os -DGLIBC_COMPAT -Wl,-z,stack-size=1048576 -Wl,--no-as-needed -lglibc-compat"
else ifneq ($(filter y,$(BR2_PACKAGE_FULLHAN_OSDRV_FH8626) $(BR2_PACKAGE_INGENIC_OSDRV_T23_V4L2)),)
	DIVINUS_DEPENDENCIES += alsa-lib
ifeq ($(BR2_PACKAGE_INGENIC_OSDRV_T23_V4L2),y)
	DIVINUS_OPTIONS = "-rdynamic -s -Os -DDIVINUS_T23 -lasound"
# Upstream divinus.yaml targets 4K sensors with continuous MJPEG and OSD;
# T23 has 1080p default capture, 24 MiB CMA, and no OSD HAL region backend yet.
define DIVINUS_T23_CONFIG
	$(SED) '/^osd:/,/^[^ ]/s/enable: true/enable: false/' $(TARGET_DIR)/etc/divinus.yaml
	$(SED) '/^mp4:/,/^[^ ]/{s/width: 3840/width: 1920/;s/height: 2160/height: 1080/}' $(TARGET_DIR)/etc/divinus.yaml
	$(SED) '/^mjpeg:/,/^[^ ]/{s/enable: true/enable: false/;s/width: 3840/width: 640/;s/height: 2160/height: 360/}' $(TARGET_DIR)/etc/divinus.yaml
	sed -n '/^osd:/,/^[^ ]/p' $(TARGET_DIR)/etc/divinus.yaml | grep -q 'enable: false'
	sed -n '/^mp4:/,/^[^ ]/p' $(TARGET_DIR)/etc/divinus.yaml | grep -q 'width: 1920'
	sed -n '/^mp4:/,/^[^ ]/p' $(TARGET_DIR)/etc/divinus.yaml | grep -q 'height: 1080'
	sed -n '/^mjpeg:/,/^[^ ]/p' $(TARGET_DIR)/etc/divinus.yaml | grep -q 'enable: false'
endef
DIVINUS_POST_INSTALL_TARGET_HOOKS += DIVINUS_T23_CONFIG
else
	DIVINUS_OPTIONS = "-rdynamic -s -Os -DDIVINUS_V4L2 -lasound"
endif
define DIVINUS_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 755 $(DIVINUS_PKGDIR)/files/S95divinus \
		$(TARGET_DIR)/etc/init.d/S95divinus
endef
# qrscan reads Wi-Fi codes from /image.jpg, which divinus serves only with
# JPEG on. The grep fails the build if divinus.yaml changes shape and the
# sed no longer matches.
define DIVINUS_ENABLE_JPEG
	$(SED) '/^jpeg:/,/^[^ ]/s/enable: false/enable: true/' $(TARGET_DIR)/etc/divinus.yaml
	sed -n '/^jpeg:/,/^[^ ]/p' $(TARGET_DIR)/etc/divinus.yaml | grep -q 'enable: true'
endef
DIVINUS_POST_INSTALL_TARGET_HOOKS += DIVINUS_ENABLE_JPEG
else
	DIVINUS_OPTIONS = "-rdynamic -s -Os"
endif

define DIVINUS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) OPT=$(DIVINUS_OPTIONS) -C $(@D)/src
endef

define DIVINUS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/divinus.yaml

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/divinus
endef

$(eval $(generic-package))
