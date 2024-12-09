#!/bin/sh

# 读取配置文件
source /etc/mosquitto/mqtt.conf

# 发布消息到指定主题
mosquitto_pub -h "$host" \
              -p "$port" \
              -u "$username" \
              -P "$password" \
              -t "$publish_topic" \
              -m "$1" \
              -i "$client_id" \
              -k "$keepalive" \
              -c