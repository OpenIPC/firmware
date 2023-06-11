################################################################################
#
# ffmpeg-openipc | updated 2022.08.10
#
################################################################################

FFMPEG_OPENIPC_VERSION = 4.4.2
FFMPEG_OPENIPC_SOURCE = ffmpeg-$(FFMPEG_OPENIPC_VERSION).tar.xz
FFMPEG_OPENIPC_SITE = http://ffmpeg.org/releases

FFMPEG_OPENIPC_INSTALL_STAGING = NO

FFMPEG_OPENIPC_LICENSE = LGPL-2.1+, libjpeg license
FFMPEG_OPENIPC_LICENSE_FILES = LICENSE.md COPYING.LGPLv2.1

FFMPEG_OPENIPC_CONF_OPTS = \
	--prefix=/usr \
	--disable-everything \
	--disable-x86asm --disable-w32threads --disable-os2threads --disable-alsa --disable-appkit \
	--disable-avfoundation --disable-bzlib --disable-coreimage --disable-iconv --disable-libxcb \
	--disable-libxcb-shm --disable-libxcb-xfixes --disable-libxcb-shape --disable-lzma \
	--disable-asm --disable-sndio --disable-sdl2 --disable-xlib --disable-zlib --disable-amf \
	--disable-audiotoolbox --disable-cuda --disable-cuvid --disable-d3d11va --disable-dxva2 \
	--disable-nvdec --disable-nvenc --disable-v4l2-m2m --disable-vaapi --disable-vdpau --disable-videotoolbox \
	--disable-avdevice --disable-swscale --disable-postproc --disable-doc --disable-runtime-cpudetect \
	--disable-bsfs --disable-iconv --disable-ffprobe --enable-gpl --enable-version3 --enable-pthreads \
	\
	--disable-swresample \
	--disable-avdevice \
	--disable-filters \
	--disable-encoders \
	--disable-decoders --enable-decoder=h264,hevc \
	--disable-muxers --enable-muxer=flv,rtsp \
	--disable-demuxers --enable-demuxer=h264,rtsp \
	--disable-parsers --enable-parser=h264,hevc \
	--disable-protocols --enable-protocol=file,rtmp,tcp \
	--disable-programs --enable-ffmpeg --enable-small


FFMPEG_OPENIPC_DEPENDENCIES += host-pkgconf

# Default to --cpu=generic for MIPS architecture, in order to avoid a
# warning from ffmpeg's configure script.
ifeq ($(BR2_mips)$(BR2_mipsel)$(BR2_mips64)$(BR2_mips64el),y)
FFMPEG_OPENIPC_CONF_OPTS += --cpu=generic
else ifneq ($(GCC_TARGET_CPU),)
FFMPEG_OPENIPC_CONF_OPTS += --cpu="$(GCC_TARGET_CPU)"
else ifneq ($(GCC_TARGET_ARCH),)
FFMPEG_OPENIPC_CONF_OPTS += --cpu="$(GCC_TARGET_ARCH)"
endif

FFMPEG_OPENIPC_CFLAGS = $(TARGET_CFLAGS)

ifeq ($(BR2_TOOLCHAIN_HAS_GCC_BUG_85180),y)
FFMPEG_OPENIPC_CONF_OPTS += --disable-optimizations
FFMPEG_OPENIPC_CFLAGS += -O0
endif

FFMPEG_OPENIPC_CONF_ENV += CFLAGS="$(FFMPEG_OPENIPC_CFLAGS)"

# Override FFMPEG_OPENIPC_CONFIGURE_CMDS: FFmpeg does not support --target and others
define FFMPEG_OPENIPC_CONFIGURE_CMDS
	(cd $(FFMPEG_OPENIPC_SRCDIR) && rm -rf config.cache && \
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	$(FFMPEG_OPENIPC_CONF_ENV) \
	./configure \
		--enable-cross-compile \
		--cross-prefix=$(TARGET_CROSS) \
		--sysroot=$(STAGING_DIR) \
		--host-cc="$(HOSTCC)" \
		--arch=$(BR2_ARCH) \
		--target-os="linux" \
		--pkg-config="$(PKG_CONFIG_HOST_BINARY)" \
		$(FFMPEG_OPENIPC_CONF_OPTS) \
	)
endef

define FFMPEG_OPENIPC_REMOVE_EXAMPLE_SRC_FILES
	rm -rf $(TARGET_DIR)/usr/share/ffmpeg/examples
endef
FFMPEG_OPENIPC_POST_INSTALL_TARGET_HOOKS += FFMPEG_OPENIPC_REMOVE_EXAMPLE_SRC_FILES

$(eval $(autotools-package))
