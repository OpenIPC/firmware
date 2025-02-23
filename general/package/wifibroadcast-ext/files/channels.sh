#!/bin/sh

if [ "$1" -eq 5 ]; then
    echo "Channel5 change."

    # Retrieve the telemetry setting from the YAML file
    tx_on_arm=$(yaml-cli -i /etc/wfb.yaml -g .telemetry.tx_on_arm)

    # Only run the TX ON ARM code if the setting is "enabled"
    if [ "$tx_on_arm" = "enabled" ]; then
        ##TX ON ARM BEGIN
        if [ "$2" -lt 1200 ]; then
            echo "Disarm detected, setting low power"
            set_alink_tx_pwr.sh 1
        elif [ "$2" -gt 1800 ]; then
            echo "ARM detected, setting high power"
            set_alink_tx_pwr.sh 9
        fi
        ##TX ON ARM END
    fi
fi
exit 0
