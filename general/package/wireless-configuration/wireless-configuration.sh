#!/bin/bash
FILES=../general/package/wireless-configuration/files
cp -f $FILES/udhcpd.conf $TARGET_DIR/etc
cp -f $FILES/interfaces $TARGET_DIR/etc/network
cp -f $FILES/netadapter $TARGET_DIR/etc/network
cp -f $FILES/netsetup $TARGET_DIR/etc/network
cp -f $FILES/wireless $TARGET_DIR/usr/sbin
