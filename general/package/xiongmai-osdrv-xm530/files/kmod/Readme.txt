驱动部分目录说明
.
|-- firmware                     #h265编码需要的文件，这个目录需要放在或链接到目录 /lib/firmware/hevcenc.fwb
|   `-- hevcenc.fwb    
|-- modules
|   |-- acodec.ko
|   |-- kmotor.ko 
|   |-- loadxm530_128M    #128M系统驱动加载脚本
|   |-- loadxm530_64M     #64M系统驱动加载脚本
|   |-- log.ko                    #日志驱动
|   |-- mmz.ko  
|   |-- mve_base.ko          #H265编码驱动
|   |-- mve_rsrc.ko            #H265编码驱动
|   |-- sdio0_sd.ko 
|   |-- xm530_h264.ko       #H264编码驱动
|   |-- xm530_isp.ko          
|   |-- xm530_rgn.ko
|   |-- xm530_vi.ko
|   |-- xm530_vo.ko
|   |-- xm_i2c.ko
|   |-- xm_i2s.ko
|   |-- xm_rtc.ko
|   `-- xm_wdt.ko
|-- readme.txt
|-- rtl8188 
|   |-- 8188eu.ko
|   |-- cfg80211.ko
|   |-- loadpublic
|   `-- wifi_pdn.ko
|-- usb               #USB驱动，使用USB接口的WiFi模块（RTL8188和XM711）需要先加载USB驱动
|   |-- dwc_common_port_lib.ko
|   `-- dwc_otg.ko
|-- xm711
|   |-- cfg80211.ko
|   |-- compat.ko
|   |-- loadpublic
|   |-- wifi_pdn.ko
|   `-- xm711.ko
`-- xm712
    |-- cfg80211.ko
    |-- compat.ko
    |-- loadpublic
    |-- wifi_pdn.ko
    `-- xm712.ko		

