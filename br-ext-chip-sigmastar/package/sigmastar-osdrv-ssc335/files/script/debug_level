#!/bin/sh
#
# OpenIPC.org | 2021.04.26
#

if [ TEST$2 != "TEST" ];
then
    export level=$2
else
    export level=0
fi

echo "$0 $1 $level"


set_level()
{
    export DEBUGLEVEL=$1
    echo $DEBUGLEVEL > /proc/mi_modules/mi_ai/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_ao/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_divp/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_rgn/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_sensor/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_shadow/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_sys/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_venc/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_vif/debug_level
    echo $DEBUGLEVEL > /proc/mi_modules/mi_vpe/debug_level
}


show_level()
{
    export SHELLCMD="cat /proc/mi_modules/mi_ai/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_ao/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_divp/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_rgn/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_sensor/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_shadow/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_sys/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_venc/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_vif/debug_level   "
    echo $SHELLCMD && $SHELLCMD
    export SHELLCMD="cat /proc/mi_modules/mi_vpe/debug_level   "
    echo $SHELLCMD && $SHELLCMD
}


case "$1" in
    set)
        set_level $level
        show_level
        ;;
    show)
        show_level
        ;;
    *)
        echo $"Usage: $0 {set|show}"
        exit 1
        ;;
esac
