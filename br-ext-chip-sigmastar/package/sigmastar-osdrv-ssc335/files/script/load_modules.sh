#!/bin/sh
#
# OpenIPC.org | 2021.04.26
#
#
# Useage: ./load [ -r|-i|-a ]
#         -r : rmmod all modules
#         -i : insmod all modules
#    default : rmmod all moules and then insmod them
#

PATH_MODULE=/opt/lib/modules/

insert_ko()
{
    insmod ${PATH_MODULE}/mhal.ko
    insmod ${PATH_MODULE}/mi_common.ko

    major=$(awk '$2=="mi" {print $1}' /proc/devices)
    minor=0

    insmod ${PATH_MODULE}/mi_sys.ko cmdQBufSize=256 logBufSize=256
    if [ $? -eq 0 ]; then
        mknod /dev/mi_sys c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_ai.ko
    if [ $? -eq 0 ]; then
        mknod /dev/mi_ai c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_ao.ko
    if [ $? -eq 0 ]; then
        mknod /dev/mi_ao c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_rgn.ko
    if [ $? -eq 0 ]; then
        mknod /dev/mi_rgn c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_divp.ko thread_priority=98
    if [ $? -eq 0 ]; then
        mknod /dev/mi_divp c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_ipu.ko
    if [ $? -eq 0 ]; then
        mknod /dev/mi_ipu c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_vpe.ko thread_priority=98
    if [ $? -eq 0 ]; then
        mknod /dev/mi_vpe c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_sensor.ko
    if [ $? -eq 0 ]; then
        mknod /dev/mi_sensor c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_vif.ko thread_priority=98
    if [ $? -eq 0 ]; then
        mknod /dev/mi_vif c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_venc.ko max_width=2304 max_height=1296  thread_priority=99
    if [ $? -eq 0 ]; then
        mknod /dev/mi_venc c $major $minor
        let minor++
    fi

    insmod ${PATH_MODULE}/mi_shadow.ko
    if [ $? -eq 0 ]; then
        mknod /dev/mi_shadow c $major $minor
        let minor++
    fi

    major=$(awk '$2=="mi_poll" {print $1}' /proc/devices)
    mknod /dev/mi_poll c $major 0

    echo hvsp2 down /config/iqfile/filter.txt /config/iqfile/filter.txt > /sys/class/mstar/mscl/hvsp

    #insmod ${PATH_MODULE}/imx307_MIPI.ko chmap=1 lane_num=2 hdr_lane_num=2
    mdev -s
}


remove_ko()
{
    rmmod mhal
    rmmod mi_common
    rmmod mi_sys
    rmmod mi_ai
    rmmod mi_ao
    rmmod mi_rgn
    rmmod mi_divp
    rmmod mi_ipu
    rmmod mi_vpe
    rmmod mi_sensor
    rmmod mi_vif
    rmmod mi_venc
    rmmod mi_shadow
    rmmod imx307_MIPI
}


if [ "$1" = "-i" ]
then
    insert_ko
fi

if [ "$1" = "-r" ]
then
    remove_ko
fi

if [ $# -eq 0 ] || [ "$1" = "-a" ]
then
    remove_ko
    insert_ko
fi


exit 0
