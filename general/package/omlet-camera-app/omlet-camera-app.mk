################################################################################
#
# omlet-camera-app
#
################################################################################

OMLET_CAMERA_APP_SITE = git@github.com:Omlet-Ltd/SmartCameraSoftware.git
OMLET_CAMERA_APP_SITE_METHOD = git
OMLET_CAMERA_APP_VERSION = 6f3886f175c9b2251edd812028691e7eb4bcc6fe
OMLET_CAMERA_APP_GIT_SUBMODULES = YES

OMLET_CAMERA_APP_INSTALL_STAGING = YES
OMLET_CAMERA_APP_LICENSE = Commercial
OMLET_CAMERA_APP_LICENSE_FILES = License

# AJL T31 board
OMLET_CAMERA_APP_CONF_OPTS += -DSENSOR_NAME=SENSOR_GC2053 -DSUPPORT_T31=1
# RL T31 board
#OMLET_CAMERA_APP_CONF_OPTS += -DSENSOR_NAME=SENSOR_GC4653 -DSUPPORT_T31=1
OMLET_CAMERA_APP_MAKE_OPTS += VERBOSE=1

OMLET_CAMERA_APP_DEPENDENCIES = \
	libglib2 \
        gst1-plugins-good \
        gst1-plugins-bad-openipc \
        json-glib\
        paho-mqtt-c

$(eval $(cmake-package))
