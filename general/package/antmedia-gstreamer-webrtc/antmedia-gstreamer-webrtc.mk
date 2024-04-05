################################################################################
#
# antmedia-gstreaamer-webrtc
#
################################################################################

ANTMEDIA_GSTREAMER_WEBRTC_SITE = https://github.com/Omlet-Ltd/AntMedia-Gstreamer-Webrtc.git
ANTMEDIA_GSTREAMER_WEBRTC_SITE_METHOD = git
ANTMEDIA_GSTREAMER_WEBRTC_VERSION = aeef10e66587fc8fb13e14c8198f8e3ea8c8346c
ANTMEDIA_GSTREAMER_WEBRTC_GIT_SUBMODULES = YES

ANTMEDIA_GSTREAMER_WEBRTC_INSTALL_STAGING = YES
ANTMEDIA_GSTREAMER_WEBRTC_LICENSE = MIT
ANTMEDIA_GSTREAMER_WEBRTC_LICENSE_FILES = License

ANTMEDIA_GSTREAMER_WEBRTC_MAKE_OPTS += VERBOSE=1

ANTMEDIA_GSTREAMER_WEBRTC_DEPENDENCIES = \
	libglib2 \
        gst1-plugins-good \
        gst1-plugins-bad \
        json-glib

$(eval $(cmake-package))
