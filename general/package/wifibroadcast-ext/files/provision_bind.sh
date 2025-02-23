#!/bin/sh

# ----------------------------------------------------------
# Exit code definitions (for readability/reference)
# ----------------------------------------------------------
# #define EXIT_ERR    1   # (Fatal errors)
# #define EXIT_BIND   2   # (File received/bind operation)

# ----------------------------------------------------------
# Cleanup function
# ----------------------------------------------------------
cleanup() {
    echo "[CLEANUP] Stopping wifibroadcast, wfb_rx, wfb_tx, wfb_tun."
    wifibroadcast stop
    killall -q wfb_rx
    killall -q wfb_tx
    killall -q wfb_tun
    sleep 1
    wifibroadcast start
}

# ----------------------------------------------------------
# Trap signals
# ----------------------------------------------------------
# - INT covers Ctrl+C
# - EXIT covers *any* exit (normal or error)
trap cleanup INT EXIT

# ----------------------------------------------------------
# (Optional) Initial cleanup to stop anything already running
# ----------------------------------------------------------
wifibroadcast stop
killall -q wfb_rx
killall -q wfb_tx
killall -q wfb_tun
sleep 1

# ----------------------------------------------------------
# Setup commands
# ----------------------------------------------------------
iw wlan0 set monitor none
iw wlan0 set channel 165 HT20
iw reg set US
sleep 1

echo "- Starting bind process..."

if ! [ -f /etc/bind.key ]
then
    echo "OoLVgEYyFofg9zdhfYPks8/L8fqWaF9Jk8aEnynFPsXNqhSpRCMbVKBFP4fCEOv5DGcbXmUHV5eSykAbFB70ew==" \
        | base64 -d > /etc/bind.key
fi

echo "- Starting wfb_rx, wfb_tx, wfb_tun"
wfb_rx -p 255 -u 5800 -K /etc/bind.key -i 10531917 wlan0 &> /dev/null &
wfb_tx -p 127 -u 5801 -K /etc/bind.key -M 1 -S 0 -L 0 \
    -k 1 -n 2 -i 10531917 wlan0 &> /dev/null &
wfb_tun -a 10.5.99.2/24 &

# Sleep needed for wfb_tun to initialize
sleep 8

