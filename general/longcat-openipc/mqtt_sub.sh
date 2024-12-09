#!/bin/sh

# 读取配置文件
source /etc/mosquitto/mqtt.conf
# 订阅消息并执行相应操作
mosquitto_sub -h "$host" \
              -p "$port" \
              -u "$username" \
              -P "$password" \
              -t "$subscribe_topic" \
              -i "$client_id" \
              -k "$keepalive" | while read -r message; do
  echo "收到消息：$message"
  # 在此处添加您处理消息的代码，例如：
  if [[ "$message" == "on" ]]; then
    echo "收到触发消息，执行脚本！"
    /usr/sbin/channels.sh 8 1000 # 在此处替换为实际要执行的脚本
  fi
  if [[ "$message" == "off" ]]; then
    echo "收到触发消息，执行脚本！"
    /usr/sbin/channels.sh 8 2000 # 在此处替换为实际要执行的脚本
  fi
done
