#!/bin/sh
#
# OpenIPC.org | 2021.04.26
#
# Useage: ./load [ -r|-i|-a ]
#         -r : rmmod all modules
#         -i : insmod all modules
#    default : rmmod all moules and then insmod them

PATH_MODULE=/opt/lib/modules/

insert_ko()
{
    insmod ${PATH_MODULE}/mhal.ko
    insmod ${PATH_MODULE}/mi_common.ko
    insmod ${PATH_MODULE}/mi_sys.ko cmdQBufSize=256 logBufSize=256
    insmod ${PATH_MODULE}/mi_ai.ko
    insmod ${PATH_MODULE}/mi_ao.ko
    insmod ${PATH_MODULE}/mi_rgn.ko
    insmod ${PATH_MODULE}/mi_divp.ko thread_priority=98
    insmod ${PATH_MODULE}/mi_ipu.ko
    insmod ${PATH_MODULE}/mi_vpe.ko thread_priority=98
    insmod ${PATH_MODULE}/mi_sensor.ko
    insmod ${PATH_MODULE}/mi_vif.ko thread_priority=98
    insmod ${PATH_MODULE}/mi_venc.ko max_width=2304 max_height=1296  thread_priority=99
    insmod ${PATH_MODULE}/mi_shadow.ko

    major=$(awk '$2=="mi_poll" {print $1}' /proc/devices)
    mknod /dev/mi_poll c $major 0
    echo hvsp2 down /config/iqfile/filter.txt /config/iqfile/filter.txt > /sys/class/mstar/mscl/hvsp
    #insmod ${PATH_MODULE}/imx307_MIPI.ko chmap=1 lane_num=2 hdr_lane_num=2
    mdev -s
}


remove_ko()
{
    rmmod imx307_MIPI
    rmmod mi_shadow
    rmmod mi_venc
    rmmod mi_vif
    rmmod mi_sensor
    rmmod mi_vpe
    rmmod mi_ipu
    rmmod mi_divp
    rmmod mi_rgn
    rmmod mi_ao
    rmmod mi_ai
    rmmod mi_sys
    rmmod mi_common
    rmmod mhal
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
