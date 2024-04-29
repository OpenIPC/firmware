################################################################################
#
# antmedia-gstreaamer-webrtc
#
################################################################################

ANTMEDIA_GSTREAMER_WEBRTC_SITE = https://github.com/USAMAWIZARD/AntMedia-Gstreamer-Webrtc.git
ANTMEDIA_GSTREAMER_WEBRTC_SITE_METHOD = git
ANTMEDIA_GSTREAMER_WEBRTC_VERSION = e0b1d550f9bbd35f72f01ccb8f27045ec0289190
ANTMEDIA_GSTREAMER_WEBRTC_GIT_SUBMODULES = YES

ANTMEDIA_GSTREAMER_WEBRTC_INSTALL_STAGING = YES
ANTMEDIA_GSTREAMER_WEBRTC_LICENSE = MIT
ANTMEDIA_GSTREAMER_WEBRTC_LICENSE_FILES = License

ANTMEDIA_GSTREAMER_WEBRTC_MAKE_OPTS += VERBOSE=1

ANTMEDIA_GSTREAMER_WEBRTC_DEPENDENCIES = \
	libglib2 \
        gst1-plugins-good \
        gst1-plugins-bad-openipc \
        json-glib

$(eval $(cmake-package))
