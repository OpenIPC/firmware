#!/bin/sh
#
# build_vtx_info.sh – Build the /etc/vtx_info.yaml configuration file
#
# This script gathers system values, reads Wi‑Fi parameters (bw, ldpc,
# stbc, and TX power settings) from /etc/wifi_profiles.yaml based on wifi_profile,
# and determines the Wi‑Fi adapter (raw driver name) solely by scanning lsusb.
#
# If the wifi_profile is not found in /etc/wifi_profiles.yaml, it falls back
# to the "default" profile.
#
# It then uses the external tool "yaml-cli" (which supports -i, -s, -g, -d)
# to set keys in /etc/vtx_info.yaml.
#

# Path to the YAML file to be built.
YAML_FILE="/etc/vtx_info.yaml"

# Create the file if it does not exist.
[ ! -f "$YAML_FILE" ] && touch "$YAML_FILE"

########################################
# Fetch dynamic values from the system #
########################################

# 1. vtx_id: use the output of ipcinfo -i; default to "UNKNOWN"
vtx_id=$(ipcinfo -i 2>/dev/null)
[ -z "$vtx_id" ] && vtx_id="UNKNOWN"

# 2. soc: read fw_printenv soc and extract the value; default to "ssc338q"
soc=$(fw_printenv soc 2>/dev/null | awk -F '=' '{print $2}')
[ -z "$soc" ] && soc="ssc338q"

# 3. sensor: read fw_printenv sensor; default to "imx335"
sensor=$(fw_printenv sensor 2>/dev/null | awk -F '=' '{print $2}')
[ -z "$sensor" ] && sensor="imx335"

# 4. build_option: extract from /etc/os-release; default "fpv"
build_option=$(grep "^BUILD_OPTION=" /etc/os-release 2>/dev/null | cut -d'=' -f2 | tr -d '"')
[ -z "$build_option" ] && build_option="fpv"

# 5. wifi_profile: use fw_printenv wifi_profile; if not defined, set default.
wifi_profile_raw=$(fw_printenv wifi_profile 2>&1)
if echo "$wifi_profile_raw" | grep -qi "not defined"; then
  wifi_profile="default"
else
  wifi_profile=$(echo "$wifi_profile_raw" | awk -F '=' '{print $2}')
fi

# 5. vtx_name: use fw_printenv vtx_name:; if not defined, set default.
vtx_name_raw=$(fw_printenv vtx_name 2>&1)
if echo "$vtx_name_raw" | grep -qi "not defined"; then
  vtx_name="default"
else
  vtx_name=$(echo "$vtx_name_raw" | awk -F '=' '{print $2}')
fi

# 6. master_groundstation: use fw_printenv master_groundstation:; if not defined, set default.
master_groundstation_raw=$(fw_printenv master_groundstation 2>&1)
if echo "$master_groundstation_raw" | grep -qi "not defined"; then
  master_groundstation="none"
else
  master_groundstation=$(echo "$master_groundstation_raw" | awk -F '=' '{print $2}')
fi


########################################
# Validate or set default wifi_profile   #
########################################

# If no wifi_profile was provided OR if the profile is not found in /etc/wifi_profiles.yaml,
# then use the "default" profile.
if [ -f /etc/wifi_profiles.yaml ]; then
    # Try to query a key (e.g. bw) from the given profile.
    test_profile=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".bw 2>/dev/null)
    if [ -z "$wifi_profile" ] || [ "$test_profile" = "Node not found." ] || [ -z "$test_profile" ]; then
         echo "Profile '$wifi_profile' not found in /etc/wifi_profiles.yaml, falling back to default." >&2
         wifi_profile="default"
    fi
else
    echo "/etc/wifi_profiles.yaml not found; proceeding without profile parameters." >&2
fi

########################################
# Determine Wi‑Fi adapter via lsusb     #
########################################

# Use lsusb to extract the raw driver name.
# The loop goes over unique USB IDs (field 6 from lsusb output) and
# sets wifi_adapter based on known card IDs.
wifi_adapter="none"
for card in $(lsusb | awk '{print $6}' | sort -u); do
    case "$card" in
        "0bda:8812" | "0bda:881a" | "0b05:17d2" | "2357:0101" | "2604:0012")
            wifi_adapter="88XXau"
            break
            ;;
        "0bda:a81a")
            wifi_adapter="8812eu"
            break
            ;;
        "0bda:f72b" | "0bda:b733")
            wifi_adapter="8733bu"
            break
            ;;
    esac
done

########################################
# Read Wi‑Fi parameters from profiles    #
########################################

# Read bandwidth (bw), LDPC, and STBC from /etc/wifi_profiles.yaml using wifi_profile.
if [ -n "$wifi_profile" ] && [ -f /etc/wifi_profiles.yaml ]; then
  wifi_bw=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".bw 2>/dev/null)
  wifi_ldpc_rx=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".ldpc_rx 2>/dev/null)
  wifi_ldpc_tx=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".ldpc_tx 2>/dev/null)
  wifi_stbc=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".stbc 2>/dev/null)
fi

# Fallback defaults if any parameter is missing.
[ -z "$wifi_bw" ] && wifi_bw="[20]"
[ -z "$wifi_ldpc_rx" ] && wifi_ldpc_rx="[0]"
[ -z "$wifi_ldpc_tx" ] && wifi_ldpc_tx="[0]"
[ -z "$wifi_stbc" ] && wifi_stbc="[0]"

########################################
# TX Power Settings from profiles      #
########################################

# For a given wifi_profile, read TX power lists.
if [ -n "$wifi_profile" ] && [ -f /etc/wifi_profiles.yaml ]; then
  tx_mcs0=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".tx_power.mcs0 2>/dev/null)
  tx_mcs1=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".tx_power.mcs1 2>/dev/null)
  tx_mcs2=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".tx_power.mcs2 2>/dev/null)
  tx_mcs3=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".tx_power.mcs3 2>/dev/null)
  tx_mcs4=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".tx_power.mcs4 2>/dev/null)
  tx_mcs5=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".tx_power.mcs5 2>/dev/null)
  tx_mcs6=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".tx_power.mcs6 2>/dev/null)
  tx_mcs7=$(yaml-cli -i /etc/wifi_profiles.yaml -g profiles."$wifi_profile".tx_power.mcs7 2>/dev/null)
fi

# Fallback TX power defaults if any list is empty.
[ -z "$tx_mcs0" ] && tx_mcs0="[1,5,10,10,10,10,10,10,10,10,10]"
[ -z "$tx_mcs1" ] && tx_mcs1="[1,5,10,10,10,10,10,10,10,10,10]"
[ -z "$tx_mcs2" ] && tx_mcs2="[1,5,10,10,10,10,10,10,10,10,10]"
[ -z "$tx_mcs3" ] && tx_mcs3="[1,5,10,10,10,10,10,10,10,10,10]"
[ -z "$tx_mcs4" ] && tx_mcs4="[1,5,10,10,10,10,10,10,10,10,10]"
[ -z "$tx_mcs5" ] && tx_mcs5="[1,5,10,10,10,10,10,10,10,10,10]"
[ -z "$tx_mcs6" ] && tx_mcs6="[1,5,10,10,10,10,10,10,10,10,10]"
[ -z "$tx_mcs7" ] && tx_mcs7="[1,5,10,10,10,10,10,10,10,10,10]"

########################################
# Video Modes (Static Tables)          #
########################################

if [ "$sensor" = "imx415" ]; then
  mode_30fps="[3840x2160,2880x1620,1920x1080,1440x810,1280x720]"
  mode_60fps="[2720x1528,1920x1080,1440x810,1360x764,1280x720]"
  mode_90fps="[1920x1080,1600x900,1440x810,1280x720,960x540]"
  mode_120fps="[1920x1080,1440x810,1280x720,1104x612,736x408]"
else
  # Default to imx335 video modes.
  mode_59fps="[1440x1080]"
  mode_60fps="[2560x1440,1920x1080,1600x900,1440x810,1280x720]"
  mode_90fps="[2208x1248,1920x1080,1440x810,1280x720,1104x624]"
  mode_120fps="[1920x1080,1600x900,1440x810,1280x720,960x540]"
fi

########################################
# Bitrate List (Based on soc)           #
########################################

if [ "$soc" = "ssc338q" ]; then
  bitrate="[4096,6144,8192,10240,12288,14336,16384]"
elif [ "$soc" = "ssc30kq" ]; then
  bitrate="[4096,6144,8192,10240,12288]"
else
  bitrate="[4096,6144,8192,10240]"
fi

########################################
# IMU Sensor detection                 #
########################################

if i2cdetect -y -r 1 2>/dev/null | grep -q "68"; then
  imu_sensor="BMI270"
else
  imu_sensor="none"
fi

########################################
# Build the YAML file using yaml-cli    #
########################################

# Top-level keys.
yaml-cli -i "$YAML_FILE" -s vtx_id "$vtx_id"
yaml-cli -i "$YAML_FILE" -s vtx_name $vtx_name
yaml-cli -i "$YAML_FILE" -s build_option "$build_option"
yaml-cli -i "$YAML_FILE" -s soc "$soc"
yaml-cli -i "$YAML_FILE" -s master_groundstation "$master_groundstation"



# Wi‑Fi section.
yaml-cli -i "$YAML_FILE" -s wifi.wifi_adapter "$wifi_adapter"
yaml-cli -i "$YAML_FILE" -s wifi.wifi_profile "$wifi_profile"
yaml-cli -i "$YAML_FILE" -s wifi.bw "$wifi_bw"
yaml-cli -i "$YAML_FILE" -s wifi.ldpc_rx "$wifi_ldpc_rx"
yaml-cli -i "$YAML_FILE" -s wifi.ldpc_tx "$wifi_ldpc_tx"
yaml-cli -i "$YAML_FILE" -s wifi.stbc "$wifi_stbc"
# TX power settings (nested mapping).
yaml-cli -i "$YAML_FILE" -s wifi.tx_power.mcs0 "$tx_mcs0"
yaml-cli -i "$YAML_FILE" -s wifi.tx_power.mcs1 "$tx_mcs1"
yaml-cli -i "$YAML_FILE" -s wifi.tx_power.mcs2 "$tx_mcs2"
yaml-cli -i "$YAML_FILE" -s wifi.tx_power.mcs3 "$tx_mcs3"
yaml-cli -i "$YAML_FILE" -s wifi.tx_power.mcs4 "$tx_mcs4"
yaml-cli -i "$YAML_FILE" -s wifi.tx_power.mcs5 "$tx_mcs5"
yaml-cli -i "$YAML_FILE" -s wifi.tx_power.mcs6 "$tx_mcs6"
yaml-cli -i "$YAML_FILE" -s wifi.tx_power.mcs7 "$tx_mcs7"

# Video section.
yaml-cli -i "$YAML_FILE" -s video.alink "[disabled,simple_alink,greg_alink]"
yaml-cli -i "$YAML_FILE" -s video.sensor "$sensor"
yaml-cli -i "$YAML_FILE" -s video.bitrate "$bitrate"
yaml-cli -i "$YAML_FILE" -s video.imu_sensor "$imu_sensor"
if [ "$sensor" = "imx415" ]; then
  yaml-cli -i "$YAML_FILE" -s video.modes."30fps" "$mode_30fps"
fi
yaml-cli -i "$YAML_FILE" -s video.modes."60fps" "$mode_60fps"
yaml-cli -i "$YAML_FILE" -s video.modes."90fps" "$mode_90fps"
yaml-cli -i "$YAML_FILE" -s video.modes."120fps" "$mode_120fps"

# Finished.
exit 0
