#!/bin/sh
# Threshold definitions:
WARNING1_THRESHOLD=80       # First warning threshold
WARNING2_THRESHOLD=90       # Second warning threshold (warning: VTX will soon throttle)
THROTTLE_THRESHOLD=100      # Throttle Level 1 threshold (100–104°C)
THROTTLE2_THRESHOLD=105     # Throttle Level 2 threshold (105–109°C)
REBOOT_THRESHOLD=110        # Reboot threshold (>=110°C)

# Define persistent state flag:
# 0 = Normal (<80°C)
# 1 = Warning level 1 (80–89°C)
# 2 = Warning level 2 (90–99°C)
# 3 = Throttle Level 1 (100–104°C)
# 4 = Throttle Level 2 (105–109°C)
# 5 = Reboot (>=110°C)
current_state=0
# Record the timestamp when the current state started
state_start_time=$(date +%s)

while true; do
    # --- Get VTX Temperature ---
    # Example output from ipcinfo --temp: "39.00"
    vtx_temp=$(ipcinfo --temp)
    vtx_int=$(echo "$vtx_temp" | cut -d. -f1)
    
    # --- Get Adapter Temperature ---
    wifi_adapter=$(yaml-cli -i /etc/vtx_info.yaml -g .wifi.wifi_adapter)
    adapter_temp=0
    if [ "$wifi_adapter" = "8733bu" ]; then
        # Expected file format: "rf_path: 0, thermal_value: 37, offset: 45, temperature: 20"
        adapter_temp=$(grep -o 'temperature: [0-9]*' /proc/net/rtl8733bu/wlan0/thermal_state | awk '{print $2}')
    elif [ "$wifi_adapter" = "88XXau" ]; then
        echo "Adapter 88XXau temperature extraction not implemented yet."
        adapter_temp=0
    elif [ "$wifi_adapter" = "8812eu" ]; then
        echo "Adapter 8812eu temperature extraction not implemented yet."
        adapter_temp=0
    else
        echo "Unknown adapter type: $wifi_adapter"
    fi

    # Fallback if adapter_temp is empty
    if [ -z "$adapter_temp" ]; then
        adapter_temp=0
    fi

    echo "VTX temperature: ${vtx_temp}°C, Adapter temperature: ${adapter_temp}°C"

    # --- Determine the Highest Temperature ---
    max_temp=$vtx_int
    if [ "$adapter_temp" -gt "$max_temp" ]; then
        max_temp=$adapter_temp
    fi

    # --- Map max_temp to a state ---
    if [ "$max_temp" -lt "$WARNING1_THRESHOLD" ]; then
        new_state=0
    elif [ "$max_temp" -lt "$WARNING2_THRESHOLD" ]; then
        new_state=1
    elif [ "$max_temp" -lt "$THROTTLE_THRESHOLD" ]; then
        new_state=2
    elif [ "$max_temp" -lt "$THROTTLE2_THRESHOLD" ]; then
        new_state=3
    elif [ "$max_temp" -lt "$REBOOT_THRESHOLD" ]; then
        new_state=4
    else
        new_state=5
    fi

    # --- Check for state change and take actions accordingly ---
    if [ "$new_state" -ne "$current_state" ]; then
        # Save previous state before updating
        old_state=$current_state
        current_state=$new_state
        state_start_time=$(date +%s)
        
        if [ "$current_state" -eq 0 ]; then
            # Recovery: only run if coming from a throttling state (3 or 4)
            if [ "$old_state" -eq 3 ] || [ "$old_state" -eq 4 ]; then
                echo "Recovered from throttling state. Running recovery commands..."
                /etc/init.d/S95majestic stop
                wifibroadcast stop
                wifibroadcast start
                /etc/init.d/S95majestic start
            fi
            echo "Normal: VTX Temp:&T WifiTemp:&W"
        
        elif [ "$current_state" -eq 1 ]; then
            echo "Warning: High temperature detected. VTX Temp:&T WifiTemp:&W" > /tmp/msposd.msg
        
        elif [ "$current_state" -eq 2 ]; then
            echo "Warning: High temperature detected. VTX will soon thermal throttle...VTX Temp:&T WifiTemp:&W" > /tmp/msposd.msg
        
        elif [ "$current_state" -eq 3 ]; then
            # Throttle Level 1 commands only if moving upward (old_state < new_state)
            if [ "$current_state" -gt "$old_state" ]; then
                if [ "$wifi_adapter" = "88XXau" ]; then
                    txpower_value="-500"
                else
                    txpower_value="500"
                fi
                iw dev wlan0 set txpower fixed $txpower_value
                echo setfps 0 30 > /proc/mi_modules/mi_sensor/mi_sensor0
            fi
            echo "Throttling VTX (level 1). Reboot imminent, return to home..VTX Temp:&T WifiTemp:&W" > /tmp/msposd.msg
        
        elif [ "$current_state" -eq 4 ]; then
            # Throttle Level 2 commands only if moving upward (old_state < new_state)
            if [ "$current_state" -gt "$old_state" ]; then
            	echo "Severe throttling VTX (level 2). Video will stop in 10sec, return to home..VTX Temp:&T WifiTemp:&W" > /tmp/msposd.msg
            	sleep 10
                /etc/init.d/S95majestic stop
            fi
            
        
        elif [ "$current_state" -eq 5 ]; then
            echo "VTX will reboot due to thermal state...VTX Temp:&T WifiTemp:&W. Rebooting in 5 seconds..."
            sleep 5
            reboot
        fi
        
    else
        # State remains the same; update the message with elapsed time.
        elapsed=$(($(date +%s) - state_start_time))
        case $current_state in
            0)
                echo "Normal: VTX Temp:&T WifiTemp:&W (State for ${elapsed} seconds)" > /tmp/msposd.msg
                ;;
            1)
                echo "Warning: High temperature detected. VTX Temp:&T WifiTemp:&W (State for ${elapsed} seconds)" > /tmp/msposd.msg
                ;;
            2)
                echo "Warning: High temperature detected. VTX will soon thermal throttle...VTX Temp:&T WifiTemp:&W (State for ${elapsed} seconds)" > /tmp/msposd.msg
                ;;
            3)
                echo "Throttling VTX (level 1). Reboot imminent, return to home..VTX Temp:&T WifiTemp:&W (State for ${elapsed} seconds)" > /tmp/msposd.msg
                ;;
            4)
                echo "Severe throttling VTX (level 2). Reboot imminent, return to home..VTX Temp:&T WifiTemp:&W (State for ${elapsed} seconds)" > /tmp/msposd.msg
                ;;
            5)
                # Reboot state should not linger.
                ;;
        esac
    fi

    sleep 5
done
