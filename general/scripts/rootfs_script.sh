#!/bin/bash
DATE=$(date +%y.%m.%d)
FILE=${TARGET_DIR}/usr/lib/os-release

echo OPENIPC_VERSION=${DATE:0:1}.${DATE:1} >> ${FILE}
date +GITHUB_VERSION="\"${GIT_BRANCH-local}+${GIT_HASH-build}, %Y-%m-%d"\" >> ${FILE}
echo BUILD_OPTION=${OPENIPC_VARIANT} >> ${FILE}
date +TIME_STAMP=%s >> ${FILE}

CONF="USES_GLIBC=y|OSDRV_T30=y|OSDRV_V85X=y|LIBV4L=y|MAVLINK_ROUTER=y|RUBYFPV=y|WIFIBROADCAST=y|AUDIO_PROCESSING_OPENIPC=y"
if ! grep -qP ${CONF} ${BR2_CONFIG}; then
	rm -f ${TARGET_DIR}/usr/lib/libstdc++*
fi

if grep -q "USES_MUSL" ${BR2_CONFIG}; then
	ln -sf libc.so ${TARGET_DIR}/lib/ld-uClibc.so.0
	ln -sf ../../lib/libc.so ${TARGET_DIR}/usr/bin/ldd
fi

LIST="${BR2_EXTERNAL_GENERAL_PATH}/scripts/excludes/${OPENIPC_SOC_MODEL}_${OPENIPC_VARIANT}.list"
if [ -f ${LIST} ]; then
	xargs -a ${LIST} -I % rm -f ${TARGET_DIR}%
fi

#############################################################################################

# 确保文件存在
MDEV_CONF="${TARGET_DIR}/etc/mdev.conf"

# 检查文件是否存在，如果存在则添加内容
if [ -f "$MDEV_CONF" ]; then
    echo "ttyUSB[3-9] root:root 660 *(/root/ok.sh)" >> "$MDEV_CONF"
else
    echo "$MDEV_CONF does not exist!"
fi


##############################################################################################
# 创建 /root/ok.sh 文件并写入内容
OK_SCRIPT="${TARGET_DIR}/root/ok.sh"

cat << 'EOF' > "$OK_SCRIPT"
#!/bin/sh

# 获取 lsusb 输出
usb_info=$(lsusb)

# 获取 ifconfig 输出
network_info=$(ifconfig)  # 使用 -a 选项来显示所有接口，包括未激活的
ok_ps=$(ps)
# 检查 lsusb 输出中是否包含特定的 USB 设备 ID
if echo "$usb_info" | grep -q "ID 2c7c:0125"; then
    # 检查 ifconfig 输出中是否包含 usb0 接口
    if echo "$network_info" | grep -q "usb0"; then
        # 什么也不做，即条件满足，无需操作
        :  # 这是一个空命令，可以用作占位符，表示这里不执行任何操作
    else
        sleep 16
        # 尝试重启 usb0 接口
        # 注意：这可能需要超级用户权限，因此可能需要使用 sudo
        echo "Attempting to restart usb0 interface..."
       
        ifdown usb0 
        ifup usb0  # 使用 && 确保 ifdown 成功后再执行 ifup
        # 注意：如果 ifdown 或 ifup 失败，脚本不会停止，但您可能希望处理这些错误情况
        ifdown ztbtowlgpc
        ifup ztbtowlgpc
        killall UDPSplitter 
        nohup /usr/bin/UDPSplitter 9021 5601 5600 > nohup.out 2>&1 &
        echo "add 192.168.192.20" | socat - UDP-DATAGRAM:127.0.0.1:9021
        echo "add 192.168.192.20" | socat - UDP-DATAGRAM:127.0.0.1:9021
        echo "add 192.168.192.20" | socat - UDP-DATAGRAM:127.0.0.1:9021
    fi
else
    echo "USB device 2c7c:0125 not found."
fi
EOF

# 给脚本添加执行权限
chmod +x "$OK_SCRIPT"
###########################################################################################################

# 修改 /etc/zerotier.conf
ZEROTIER_CONF="${TARGET_DIR}/etc/zerotier.conf"
if [ -f "$ZEROTIER_CONF" ]; then
    sed -i 's/enabled=false/enabled=true/' "$ZEROTIER_CONF"
else
    echo "$ZEROTIER_CONF does not exist!"
fi
############################################################################################################
# 修改 /etc/wfb.conf
WFB_CONF="${TARGET_DIR}/etc/wfb.conf"
if [ -f "$WFB_CONF" ]; then
    sed -i 's/channel=64/channel=161/' "$WFB_CONF"
    sed -i 's/driver_txpower_override=1/driver_txpower_override=20/' "$WFB_CONF"
else
    echo "$WFB_CONF does not exist!"
fi

#################################################################################################################

#修改MAC

cat << EOF | tee "${TARGET_DIR}/etc/network/interfaces.d/eth0"
iface eth0 inet dhcp
    hwaddress ether $(fw_printenv -n ethaddr || echo 26:7c:a0:57:91:e8)
EOF


################################################################################################################


# 复制 UDPSplitter
cp ${GITHUB_WORKSPACE}/general/scripts/UDPSplitter ${TARGET_DIR}/usr/bin/UDPSplitter
chmod +x "${TARGET_DIR}/usr/bin/UDPSplitter"

#################################################################################################################

#修改/usr/sbin/channels.sh
CHSH="${TARGET_DIR}/usr/sbin/channels.sh"
cat << 'EOF' > "$CHSH"
#!/bin/sh

echo $1 $2 >> /tmp/channels.log
# channel 8
if [ $1 -eq 8 ]; then
    if [ $2 -gt 1600 ]; then
      /usr/sbin/ircut.sh on 23 24
    fi

    
    if [ $2 -lt 1500 ]; then
      /usr/sbin/ircut.sh off 23 24
    fi
fi
exit 1
EOF

#######################################################################################################################

# 复制 mosquitto配置
cp -rf "${GITHUB_WORKSPACE}/general/longcat-openipc/mosquitto" "${TARGET_DIR}/etc/"
cp "${GITHUB_WORKSPACE}/general/longcat-openipc/mqtt_pub.sh" "${TARGET_DIR}/usr/sbin/"
chmod +x "${TARGET_DIR}/usr/sbin/mqtt_pub.sh"
cp "${GITHUB_WORKSPACE}/general/longcat-openipc/mqtt_sub.sh" "${TARGET_DIR}/usr/sbin/"
chmod +x "${TARGET_DIR}/usr/sbin/mqtt_sub.sh"
# cp "${GITHUB_WORKSPACE}/general/longcat-openipc/passwd"  "${TARGET_DIR}/etc/mosquitto/"

cp "${GITHUB_WORKSPACE}/general/longcat-openipc/"{1080.sh,1080b.sh,1080c.sh,3K.sh,4K.sh,720.sh,720b.sh,720c.sh,720d.sh,kill.sh} "${TARGET_DIR}/root/"
# 给/root所有脚本执行权限
find "${TARGET_DIR}/root" -name "*.sh" -exec chmod +x {} \;
#######################################################################################################################
#修改时间
echo "Asia/Shanghai" | tee "${TARGET_DIR}/etc/timezone"
echo "CST-8" | tee "${TARGET_DIR}/etc/TZ"

cat << EOF | tee "${TARGET_DIR}/etc/ntp.conf"
server ntp.aliyun.com iburst
server time1.aliyun.com iburst
server time2.aliyun.com iburst
server time3.aliyun.com iburst
EOF


#######################################################################################################################

#修改majestic.yaml
cat << EOF | tee "${TARGET_DIR}/etc/majestic.yaml"
system:
  webAdmin: enabled
  buffer: 1024
image:
  mirror: false
  flip: false
  rotate: 0
  contrast: auto
  hue: 50
  saturation: 50
  luminance: auto
osd:
  enabled: false
  template: "%Y-%m-%d %H:%M:%S"
  posX: 16
  posY: 16
nightMode:
  enabled: true
  colorToGray: true
  irSensorPinInvert: true
  irCutSingleInvert: false
  irCutPin1: 23
  irCutPin2: 24
  irSensorPin: 60
  dncDelay: 6
records:
  enabled: false
  path: /mnt/mmcblk0p1/%F/%H.mp4
  maxUsage: 95
video0:
  enabled: true
  size: 1920x1080
  fps: 60
  rcMode: cbr
  codec: h265
  bitrate: 4096
  gopSize: 1.5
video1:
  enabled: false
jpeg:
  enabled: true
mjpeg:
  size: 640x360
  fps: 5
  bitrate: 1024
audio:
  enabled: true
  volume: auto
  srate: 8000
  speakerPinInvert: false
  outputEnabled: true
  speakerPin: 39
  outputVolume: 100
  codec: opus
rtsp:
  enabled: true
  port: 554
hls:
  enabled: false
youtube:
  enabled: false
motionDetect:
  enabled: false
  visualize: false
  debug: false
ipeye:
  enabled: false
watchdog:
  enabled: true
  timeout: 300
isp:
  exposure: 20
  aGain: 40
  sensorConfig: /etc/sensors/imx415_fpv.bin
outgoing:
  enabled: true
  server: udp://127.0.0.1:5600
  naluSize: 1200
EOF

#######################################################################################################################
#  msposd加入日期 fps 和摄像头温度 
sed -i '/--out 127.0.0.1:$(($port_tx + 1)) -osd -r "$fps" --ahi "$ahi" > \/dev\/null &/a echo "&L20 &F30 CPU:&C &B temp:&T &t " >/tmp/MSPOSD.msg' "${TARGET_DIR}/usr/bin/telemetry"
cp "${TARGET_DIR}/usr/share/fonts/font_btfl.png" "${TARGET_DIR}/usr/share/fonts/font.png "
cp "${TARGET_DIR}/usr/share/fonts/font_btfl_hd.png"   "${TARGET_DIR}/usr/share/fonts/font_hd.png" 
#######################################################################################################################
# 修改telemetry脚本 启动mavlink路由器
sed -i '/--out 127.0.0.1:$(($port_tx + 1)) -osd -r "$fps" --ahi "$ahi" > \/dev\/null &/a\
\t\t\t\t\tmavlink-routerd -c /etc/mavlink.conf > /dev/null 2>&1 &' ${TARGET_DIR}/usr/bin/telemetry


########################################################################################################################
