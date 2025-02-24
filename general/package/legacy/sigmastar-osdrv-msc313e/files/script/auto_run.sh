#!/bin/sh

function fn_msc313_evn()
{
    mkdir -p /data/audio
    mkdir -p /data/cfg
    mkdir -p /data/misc
    mkdir -p /data/property

    cp -rf /ipc/data/audio/* /data/audio
    cp -rf /ipc/data/cfg/* /data/cfg
    cp -rf /ipc/data/misc/* /data/misc

    ### param
    ## TCP/IP configuration
    echo 163840 > /proc/sys/net/core/rmem_max
    echo 327680 > /proc/sys/net/core/wmem_max
    echo '1488 1984 2976' > /proc/sys/net/ipv4/tcp_mem
    echo '4096 87380 325120' > /proc/sys/net/ipv4/tcp_rmem
    echo '16384 327680 1048576' > /proc/sys/net/ipv4/tcp_wmem

    ulimit -c unlimited
    echo /ipc/app/core.%e.%t > /proc/sys/kernel/core_pattern
    echo 2 > /sys/class/mstar/mvhe/rct
    #echo 3 > /sys/class/mstar/mmfe/rct
    echo 1 > /sys/class/mstar/msys/dmem_realloc

    # stack size 6M
    ulimit -s 6144

    #disable overcommit
    echo 0 > /proc/sys/vm/overcommit_memory
    echo 50 > /proc/sys/vm/overcommit_ratio

    ### platform
    chip=`cat /sys/devices/soc0/machine | awk '{print $2}'`

    ### MSC313E
    setprop mi.vi.src 2
    setprop mi.sys.shrink_mem 1
    setprop mi.vi.img.sub 0
    setprop mi.venc.bufcnt 2
    setprop mi.venc.sub.bufcnt 2
    
    setprop mi.vi.bufcnt 3
    setprop mi.vi.sub.bufcnt 3
    setprop mi.vi.sub.width 640
    setprop mi.vi.sub.height 480
   
    # SW OSD
    #setprop mi.osd.stretch 1
    setprop mi.osd.gop.use 0
    setprop mstar.omx.gop.disable 1
    
    echo D > /sys/class/mstar/mhvsp1/fbmg
    echo 9 > /sys/class/mstar/mhvsp1/fbmg
    #echo 0 > /sys/class/mstar/mscldma1/db
    #echo D40 > /sys/class/mstar/mhvsp1/dbgmg
	
    if [ -f /opt/iqserver.sh ]; then
        /opt/iqserver.sh
    else
        # 固定编码内存不做它用
        # 根据 cat /sys/class/mstar/msys/dmem 这个info来配置
        #echo "Not fix memory"
        setprop mi.venc.bufratio 50
        setprop mi.venc.img.bufratio 40
        setprop mi.video.height.force.aligned32 1
			
        dmem -req 0x00016800 VSPL-I0P3B2
        dmem -req 0x00016800 VSPL-I0P3B1
        dmem -req 0x00016800 VSPL-I0P3B0
        dmem -req 0x00025800 VENC-49
        dmem -req 0x00025800 VENC-48
        dmem -req 0x00070800 VSPL-I0P1B2
        dmem -req 0x00070800 VSPL-I0P1B1
        dmem -req 0x00070800 VSPL-I0P1B0
        dmem -req 0x000FF000 VENC-33
        dmem -req 0x000FF000 VENC-32
        dmem -req 0x00357000 MS-02
        dmem -req 0x00357000 MS-01
        dmem -req 0x00357000 MS-00
        dmem -req 0x00070800 VSPL-I0P0B2
        dmem -req 0x00070800 VSPL-I0P0B1
        dmem -req 0x00070800 VSPL-I0P0B0
        dmem -req 0x000FF000 SCL_MCNR_M
        dmem -req 0x003FC000 SCL_MCNR_YC
        dmem -req 0x00000400 DLC_MEM
        dmem -req 0x000088E0 ISP_MLOAD
        dmem -req 0x00014000 pcmC0D0c
        dmem -req 0x00018000 pcmC0D0p
        dmem -req 0x00000100 BDMA_FSP_WBUFF
        dmem -req 0x0001B120 ISP_base
        dmem -req 0x00002000 EMAC_BUFF

        echo "VSPL-I0P3B2" > /sys/class/mstar/msys/fixed_dmem
        echo "VSPL-I0P3B1" > /sys/class/mstar/msys/fixed_dmem
        echo "VSPL-I0P3B0" > /sys/class/mstar/msys/fixed_dmem
        echo "VENC-49" > /sys/class/mstar/msys/fixed_dmem  
        echo "VENC-48" > /sys/class/mstar/msys/fixed_dmem   
        echo "VSPL-I0P1B2" > /sys/class/mstar/msys/fixed_dmem 
        echo "VSPL-I0P1B1" > /sys/class/mstar/msys/fixed_dmem
        echo "VSPL-I0P1B0" > /sys/class/mstar/msys/fixed_dmem
        echo "VENC-33" > /sys/class/mstar/msys/fixed_dmem
        echo "VENC-32" > /sys/class/mstar/msys/fixed_dmem 
        echo "MS-02" > /sys/class/mstar/msys/fixed_dmem     
        echo "MS-01" > /sys/class/mstar/msys/fixed_dmem     
        echo "MS-00" > /sys/class/mstar/msys/fixed_dmem     
        echo "VSPL-I0P0B2" > /sys/class/mstar/msys/fixed_dmem
        echo "VSPL-I0P0B1" > /sys/class/mstar/msys/fixed_dmem
        echo "VSPL-I0P0B0" > /sys/class/mstar/msys/fixed_dmem
        echo "SCL_MCNR_M" > /sys/class/mstar/msys/fixed_dmem
        echo "SCL_MCNR_YC" > /sys/class/mstar/msys/fixed_dmem
        echo "DLC_MEM" > /sys/class/mstar/msys/fixed_dmem    
        echo "ISP_MLOAD" > /sys/class/mstar/msys/fixed_dmem  
        echo "pcmC0D0c" > /sys/class/mstar/msys/fixed_dmem   
        echo "pcmC0D0p" > /sys/class/mstar/msys/fixed_dmem   
        echo "BDMA_FSP_WBUFF" > /sys/class/mstar/msys/fixed_dmem 
        echo "ISP_base" > /sys/class/mstar/msys/fixed_dmem       
        echo "EMAC_BUFF" > /sys/class/mstar/msys/fixed_dmem      

        echo 1 > /sys/class/mstar/msys/fixed_dmem
        echo 0 > /sys/class/mstar/msys/dmem_realloc
    fi
}

function fn_setip() 
{
    if grep -q 'root=/dev/nfs' /proc/cmdline; then
        echo "run on nfs, no flash mount!"
        return 0
    fi

    if [ -f /opt/conf/config.xml ]; then
        dev_ip=`xml_shuttle -cr -k /cfg/eth/ip /opt/conf/config.xml`
        dev_ip=`echo $dev_ip| awk -F' ' '{print $3}'`
        ifconfig eth0 $dev_ip
    fi
}

function fn_hwclock() 
{
    test -e /var/run/hwclock
    if [ 1 -eq $? ]; then
        busybox hwclock -s
        echo $? > /var/run/hwclock
    fi
}

function fn_reboot()
{
    logger -t autorun "exit jco_server"
    cp /tmp/messages /opt/log/messages.reboot
    sync
    echo "reboot as jco_server exit"
    reboot -f
}

function fn_load_drv()
{
    insmod /ipc/drv/gd25q.ko
}

function fn_main()
{
    # init mac
    # macmac=`ifconfig eth7 | grep "inet addr" | cut -d : -f 2 | cut -d ' ' -f 1`
    # macmac=`echo $macmac | sed -e 's/^192/8/g' -e 's/\.//g'  | xargs printf "%012d"`
    # echo $macmac | sed 's/../&-/g' | sed 's/-$//g'
    # /sbin/ifconfig eth0 down
    # /sbin/ifconfig eth0 hw ether `/usr/bin/fw_printenv -n ethaddr`

    # init ip when no bootip
    bootip=`ifconfig eth0 | grep "inet addr" | cut -d : -f 2 | cut -d ' ' -f 1`
    [ -z "${bootip}" ] && { ifconfig eth0 192.168.1.217; ifconfig lo up; } 

    echo "PATH: ${PATH}"
    echo "LD_LIBRARY_PATH: ${LD_LIBRARY_PATH}"

    # sync ubifs_bgt0_0 block with high-cpu-usage, 
    # reboot in client_fee_watchdog(), reboot here will block
    # speed of I/O write() is .33M/s, after ENCODE exit 21M/14 -> 1.5M/s
    # flush when dirty upto 1.2M or .6M, freeTotal is 60M
    echo   5 >/proc/sys/vm/dirty_ratio
    echo   3 >/proc/sys/vm/dirty_background_ratio       # 1.8M
    #cho 300 >/proc/sys/vm/dirty_writeback_centisecs    # 0.9M default 500
    #cho 500 >/proc/sys/vm/dirty_expire_centisecs       # 1.8M default 3000

    test -x /opt/etc/local.rc && /opt/etc/local.rc
    if ! ps | grep '[t]elnetd'; then
        telnetd -p9527
        (sleep 300; ps|awk '/[p]9527/ {print $1}' | xargs kill -9) &
    fi

    fn_load_drv

    #/sbin/udevadm trigger
    fn_setip
    fn_hwclock
    
    while :; do
        echo "
        +------------------------------------------------+
             Hello Server World @`date +%F.%T`!!
        +------------------------------------------------+
        "
        
        logger -t autorun "start jco_server @${count:-0}" 
        rm -rf /opt/upgrade/*
        /ipc/app/jco_server 
	    let count++
	    sleep 2
        [ "${count:-0}" -ge 5 ] && reboot -f && fn_reboot
    done
}

fn_msc313_evn
fn_main $@
