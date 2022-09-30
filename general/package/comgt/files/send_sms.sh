#!/bin/sh

MODEM=/dev/ttyUSB2
DESTNUM="+7978XXXXXX"
MESSAGE="Hi, there"


# We need to put sleep 1 to slow down commands for modem to process
#
echo -e "ATZ\r" >${MODEM}
sleep 1
#
echo -e "AT+CMGF=1\r" >${MODEM}
sleep 1
#
echo -e "AT+CMGS=\"$DESTNUM\"\r" >${MODEM}
sleep 1
#
echo -e "${MESSAGE}\x1A" >${MODEM}
sleep 1

