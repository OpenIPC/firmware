#!/bin/sh
# Define temperature thresholds:
NORMAL_THRESHOLD=85    # Normal if below 85°C
REBOOT_THRESHOLD=100   # Reboot if 100°C or higher

# --- Get Adapter info ---
for card in $(lsusb | awk '{print $6}' | sort | uniq); do
    case "$card" in
        "0bda:8812" | "0bda:881a" | "0b05:17d2" | "2357:0101" | "2604:0012")
            driver=88XXau
            ;;
        "0bda:a81a")
            driver=8812eu
            ;;
        "0bda:f72b" | "0bda:b733")
            driver=8733bu
            ;;
    esac
done
wifi_adapter=""$driver""
echo "Wifi adapter found: $driver"

while true; do
    # --- Get VTX Temperature ---
    # Example output from ipcinfo --temp: "39.00"
    vtx_temp=$(ipcinfo --temp)
    vtx_int=$(echo "$vtx_temp" | cut -d. -f1)
    
    adapter_temp=0
    if [ "$wifi_adapter" = "8733bu" ]; then
        # Expected format: "rf_path: 0, thermal_value: 37, offset: 45, temperature: 20"
        adapter_temp=$(grep -o 'temperature: [0-9]*' /proc/net/rtl8733bu/wlan0/thermal_state | awk '{print $2}')
    elif [ "$wifi_adapter" = "88XXau" ]; then
        echo "Adapter 88XXau temperature extraction not implemented yet."
        adapter_temp=0
    elif [ "$wifi_adapter" = "8812eu" ]; then
        if [ -f /proc/net/rtl88x2eu/wlan0/thermal_state ]; then
            # Extract both temperature values and choose the highest
            adapter_temp=$(grep -o 'temperature: [0-9]*' /proc/net/rtl88x2eu/wlan0/thermal_state | awk '{print $2}' | sort -n | tail -1)
        else
            echo "Thermal state file for 8812eu not found."
            adapter_temp=0
        fi
    else
        echo "Unknown adapter type: $wifi_adapter"
    fi

    # Fallback if adapter_temp is empty
    if [ -z "$adapter_temp" ]; then
        adapter_temp=0
    fi

    echo "VTX temperature: ${vtx_temp}°C, Adapter temperature: ${adapter_temp}°C"

    # --- Determine the Highest Temperature ---
    if [ "$adapter_temp" -gt "$vtx_int" ]; then
        max_temp=$adapter_temp
    else
        max_temp=$vtx_int
    fi

    # --- Determine the current state based on max_temp ---
    if [ "$max_temp" -lt "$NORMAL_THRESHOLD" ]; then
        echo "All is well... Nothing to do" > /dev/null
    elif [ "$max_temp" -lt "$REBOOT_THRESHOLD" ]; then
        echo "Warning: High temperature was detected.\nVTX Temp:&T WifiTemp:&W &L30 &G8 &F18" > /tmp/MSPOSD.msg
    else
        echo "VTX will reboot due to thermal state...\nVTX Temp:&T WifiTemp:&W &L30 &G8 &F18.\nRebooting in 15 seconds..." > /tmp/MSPOSD.msg
        sleep 15
        reboot
    fi

    sleep 5
done
