#!/bin/sh

gpio=$(gpiofind red-led | awk '{print $2}')

n=30

gpio toggle ${gpio} | logger -t gpio

temp=$(mktemp)

while true ; do
    echo "timeout $n"
    gpio toggle ${gpio} | logger -t gpio
    if [ "$n" -le 0 ]; then
        logger -t qrscan "Recognition timeout exceeded, reboot camera and try again..."
        gpio toggle ${gpio} | logger -t gpio
        exit 1
    fi
    timeout 1 wget -q -O /tmp/image.jpg http://127.0.0.1/image.jpg
    data=$(qrscan -p /tmp/image.jpg)
    if [[ -n "$data" ]] && $(echo "$data" | grep -q WIFI); then
        while [ -e $temp ]; do gpio toggle ${gpio}; sleep 0.2; done >/dev/null 2>&1 &

        # 去除开头的 "WIFI:" 前缀
        CONTENT="${data#WIFI:}"

        # 去除末尾的 ";;" 结束符
        CONTENT="${CONTENT%%;;}"

        # 初始化变量（标准字段）
        T=""   # 认证类型
        S=""   # SSID
        P=""   # 密码
        H=""   # 隐藏网络

        # 初始化变量（扩展字段 - 静态 IP）
        IP=""
        MASK=""
        GW=""
        DNS=""

        # 使用 IFS 分割字符串，解析每个 key:value 对
        OLD_IFS="$IFS"
        IFS=';'
        for pair in $CONTENT; do
            # 跳过空字符串
            [ -z "$pair" ] && continue

            # 提取 key 和 value
            key="${pair%%:*}"
            value="${pair#*:}"

            # 处理转义字符（将 \: 还原为 :）
            value=$(echo "$value" | sed 's/\\:/:/g')

            case "$key" in
                T)      T="$value" ;;
                S)      S="$value" ;;
                P)      P="$value" ;;
                H)      H="$value" ;;
                IP)     IP="$value" ;;
                MASK)   MASK="$value" ;;
                GW)     GW="$value" ;;
                DNS)    DNS="$value" ;;
                *)      echo "警告: 忽略未知字段 $key" ;;
            esac
        done
        IFS="$OLD_IFS"

        # ========== 输出解析结果 ==========
        echo "========== 解析结果 =========="
        echo "Wi-Fi 配置:"
        echo "  SSID      : $S"
        echo "  密码      : $P"
        echo "  隐藏网络  : ${H:-false}"

        # 判断是否有静态 IP 配置
        if [ -n "$IP" ]; then
            echo ""
            echo "静态 IP 配置 (已启用):"
            echo "  IP 地址   : $IP"
            HAS_STATIC=1
        else
            echo ""
            echo "IP 分配方式: DHCP"
            HAS_STATIC=0
        fi

        fw_setenv wlanssid $S
        fw_setenv wlanpass $P
        logger -t qrscan "Recognition successfully, wlanssid and wlanpass is writed to env. Reboot required."
        curl --data-binary @/usr/share/openipc/sounds/ready_48k.pcm http://localhost/play_audio
        #reboot -f
        ifup wlan0
        break
    fi
    sleep 1
    n=$((n - 1))
done

rm $temp
