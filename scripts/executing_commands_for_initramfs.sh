#!/bin/bash
#
# Please check BR2_ROOTFS_POST_BUILD_SCRIPT option in config
#

echo 'Note: Deleting unused files from the image..'
#
rm -f ${TARGET_DIR}/etc/init.d/S01syslogd
rm -f ${TARGET_DIR}/etc/init.d/S10modules
rm -f ${TARGET_DIR}/etc/init.d/S21rngd
rm -f ${TARGET_DIR}/etc/init.d/S40mdev
rm -f ${TARGET_DIR}/etc/init.d/S49ntpd
rm -f ${TARGET_DIR}/etc/init.d/S50dropbear
rm -f ${TARGET_DIR}/etc/init.d/S50snmpd
#
rm -rf ${TARGET_DIR}/etc/ssl
#
