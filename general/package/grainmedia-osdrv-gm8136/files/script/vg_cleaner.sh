#!/bin/sh
# Grain Media Video Graph Cleanup Script

video_frontend=soih42
# GM8210, GM828x: nvp1918, nvp1118, cx26848, cx26848_960h, tw2968, mv4101
# GM8139, GM8136: soih42, ov9715, ov5653, ar0331, mt9m034, ov9714
# GM8139, GM8136: imx122, imx222, imx138, imx236, ar0835, ar0330, mt9p031, imx238, mn34220

chipver=`head -1 /proc/pmu/chipver`
chipid=`echo $chipver | cut -c 1-4`
cpu_enum=`grep -A 3 'cpu_enum' /proc/pmu/attribute | grep 'Attribute value' | cut -c 18`
# cpu_enum = 0(host_fa726), 1(host_fa626), 2(host_7500), 3(dev_fa726), 4(dev_fa626)

/sbin/rmmod vpd_master.ko
/sbin/rmmod vpd_slave.ko
/sbin/rmmod loop_comm.ko
/sbin/rmmod gs.ko
/sbin/rmmod audio_drv.ko
/sbin/rmmod codec.ko
/sbin/rmmod osd_dispatch.ko
/sbin/rmmod fscaler300.ko
/sbin/rmmod sw_osg.ko

if [ "$chipid" != 8135 ];then
	/sbin/rmmod mp4e_rc.ko
	/sbin/rmmod fmpeg4_drv.ko
fi

/sbin/rmmod fmjpeg_drv.ko
/sbin/rmmod decoder.ko
/sbin/rmmod favc_rc.ko
/sbin/rmmod favc_enc.ko
/sbin/rmmod fmcp_drv.ko

if [ "$chipid" == "8139" ] || [ "$chipid" == "8138" ] || [ "$chipid" == "8137" ]; then
    /sbin/rmmod /lib/modules/vcap300_isp.ko
    /sbin/rmmod /lib/modules/vcap0.ko
    /sbin/rmmod /lib/modules/vcap300_common.ko
    /sbin/rmmod /lib/modules/fisp_$video_frontend.ko
    /sbin/rmmod /lib/modules/fisp_algorithm.ko		
    /sbin/rmmod /lib/modules/fisp320.ko		
    /sbin/rmmod /lib/modules/ft3dnr.ko		
    /sbin/rmmod /lib/modules/adda302.ko		
    /sbin/rmmod /lib/modules/fe_common.ko		
    /sbin/rmmod /lib/modules/ftpwmtmr010.ko		
    /sbin/rmmod /lib/modules/think2d.ko		
    /sbin/rmmod /lib/modules/sar_adc.ko		
elif [ "$chipid" == "8136" ] || [ "$chipid" == "8135" ]; then
    /sbin/rmmod vcap300_isp.ko
    /sbin/rmmod vcap0.ko
    /sbin/rmmod vcap300_common.ko
    /sbin/rmmod fisp_$video_frontend.ko
    /sbin/rmmod fisp_algorithm.ko
    /sbin/rmmod fisp328.ko
    /sbin/rmmod ft3dnr200.ko
    /sbin/rmmod adda308.ko
    /sbin/rmmod fe_common.ko
    /sbin/rmmod ftpwmtmr010.ko
    /sbin/rmmod think2d.ko
    /sbin/rmmod sar_adc.ko
fi
