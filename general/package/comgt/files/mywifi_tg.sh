#!/bin/sh
#
# Telegram Sender v.0.1
#
# 2017.09.15 | Simple text sender realisation
# 2017.09.16 | Nagios test done


TOKEN="4938561407:AAGcMIK1zBFW40AT5436vPvEUsFP6BmxhI"
RUPOR="-10013071148102"
SDATE=$(date "+%Y-%m-%d %H:%M:%S")
ALARM=`echo -e "\xE2\x98\x8E K-Telecom"`
STAMP=$(date -u +%s)
NOMER=$(echo $1 | tr -d '+')


send() {
  #
  if [ -z "${NOMER}" ] ; then
    NOMER="anonymouse"
  fi
  #
  curl -k -o - --connect-timeout 30 --max-time 30 -X POST \
    https://api.telegram.org/bot${TOKEN}/sendMessage -d chat_id="${RUPOR}" -d disable_notification="true" -d text="${ALARM} ${NOMER} | ${SDATE}" >/dev/null 2>&1
  #
  logger -t ringer "${STAMP} Received ring from: ${NOMER} - ${MYWIFI}"
  #
}

send

