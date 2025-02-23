#!/bin/sh

##Add code to print out temp etc for the web interface.
#soc temp, wifi temp, current wfb settings, current wifi tx_pwr
ipcinfo -t >> /tmp/vtx.log
ipcinfo -t > /tmp/www_info
