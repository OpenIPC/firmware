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

    insmod ${PATH_MODULE}/imx307_MIPI.ko chmap=1 lane_num=2 hdr_lane_num=2
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




) 1280x 720,  30 fps max --> 192function:parese_Cmdline,pCmd_Section:0x3fe0000
0x1080 [m--] WDR sn lx len:mmap->u32Size:0x7fe0000,but kernel len:0x3fe0000,fail!!!!

нет ipu

 mi_ai 187375 0 - Live 0xbfb2f000 (PO)
 mi_divp 48340 0 - Live 0xbfb1e000 (PO)
mi_disp 93595 0 - Live 0xbfaff000 (PO)
mi_panel 24836 0 - Live 0xbfaf4000 (O)
 mi_venc 155868 0 - Live 0xbfac3000 (PO)
 mi_vif 39235 0 - Live 0xbfab4000 (PO)
mi_gyro 11246 0 - Live 0xbfaad000 (PO)
 mi_shadow 34375 0 - Live 0xbfa9f000 (PO)
 mi_vpe 131267 0 - Live 0xbfa76000 (PO)
 mi_rgn 74250 2 mi_divp,mi_vpe, Live 0xbfa5d000 (PO)
 mi_ao 72914 0 - Live 0xbfa45000 (PO)
 mi_sensor 21952 0 - Live 0xbfa3b000 (PO)
 mi_sys 411756 12 mi_ai,mi_divp,mi_disp,mi_panel,mi_venc,mi_vif,mi_gyro,mi_shadow,mi_vpe,mi_rgn,mi_ao,mi_sensor, Live 0xbf9c2000 (PO)
 mi_common 5599 13 mi_ai,mi_divp,mi_disp,mi_panel,mi_venc,mi_vif,mi_gyro,mi_shadow,mi_vpe,mi_rgn,mi_ao,mi_sensor,mi_sys, Live 0xbf9bd000 (PO)
 mhal 1471673 12 mi_ai,mi_divp,mi_disp,mi_panel,mi_venc,mi_vif,mi_gyro,mi_vpe,mi_rgn,mi_ao,mi_sensor,mi_sys, Live 0xbf804000 (PO)
ms_notify 1281 0 - Live 0xbf800000
