#!/bin/sh
#
# set_bitrate.sh
#
# Usage:
#    set_bitrate.sh <target_bitrate_in_kbps> [max_mcs] [--cap <cap_value>] [--max_bw <20|40>] [--direction <initial|increased|decreased|unchanged>] [--tx_pwr <tx_power_value>]
#
# (Additional usage info…)
#

# --- Parse arguments ---
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <target_bitrate_in_kbps> [max_mcs] [--cap <cap_value>] [--max_bw <20|40>] [--direction <initial|increased|decreased|unchanged>] [--tx_pwr <tx_power_value>]"
    exit 1
fi

TARGET="$1"
shift

# Default max_mcs is 5.
max_mcs=5
if [ "$#" -gt 0 ]; then
    case "$1" in
        --*) ;;  # if next arg starts with --
        *) max_mcs="$1"; shift;;
    esac
fi

# Default cap is 20000 kbps unless overridden.
CAP=20000
# Default max_bw for candidate search is 40 (both 20 and 40 will be searched).
MAX_BW=40

# Default direction. If not provided, we assume "initial"
DIRECTION="initial"

# Default TX_PWR is set to 5 unless overridden.
TX_PWR=5

while [ "$#" -gt 0 ]; do
    case "$1" in
        --cap)
            shift
            if [ -z "$1" ]; then
                echo "Error: --cap requires a value."
                exit 1
            fi
            CAP="$1"
            shift
            ;;
        --max_bw)
            shift
            if [ -z "$1" ]; then
                echo "Error: --max_bw requires a value (20 or 40)."
                exit 1
            fi
            if [ "$1" != "20" ] && [ "$1" != "40" ]; then
                echo "Error: --max_bw must be either 20 or 40."
                exit 1
            fi
            MAX_BW="$1"
            shift
            ;;
        --direction)
            shift
            if [ -z "$1" ]; then
                echo "Error: --direction requires a value (initial, increased, decreased, or unchanged)."
                exit 1
            fi
            case "$1" in
                initial|increased|decreased|unchanged)
                    DIRECTION="$1"
                    ;;
                *)
                    echo "Error: Unknown direction: $1. Valid options are: initial, increased, decreased, or unchanged."
                    exit 1
                    ;;
            esac
            shift
            ;;
        --tx_pwr)
            shift
            if [ -z "$1" ]; then
                echo "Error: --tx_pwr requires a value."
                exit 1
            fi
            TX_PWR="$1"
            shift
            ;;
        *)
            echo "Unknown argument: $1"
            exit 1
            ;;
    esac
done

# --- Read FEC settings from /etc/wfb.yaml ---
fec_n=$(yaml-cli -i /etc/wfb.yaml -g .broadcast.fec_n)
fec_k=$(yaml-cli -i /etc/wfb.yaml -g .broadcast.fec_k)
if [ -z "$fec_n" ] || [ -z "$fec_k" ]; then
    echo "Error: Could not read FEC settings from /etc/wfb.yaml."
    exit 1
fi
fec_ratio="${fec_k}/${fec_n}"

# --- Read allowed bandwidths from /etc/vtx_info.yaml ---
bw_array=$(yaml-cli -i /etc/vtx_info.yaml -g .wifi.bw)
max_bw_allowed=$(echo "$bw_array" | sed 's/[][]//g' | awk -F, '{print $NF}' | tr -d ' ')
if [ -z "$max_bw_allowed" ]; then
    echo "Error: Could not read allowed bandwidths from /etc/vtx_info.yaml."
    exit 1
fi
if [ "$max_bw_allowed" -ge 40 ]; then
    ALLOWED_BW="40"
else
    ALLOWED_BW="20"
fi

# --- Call bitrate_calculator.sh ---
RESULT=$(bitrate_calculator.sh "$TARGET" "$fec_ratio" "$max_mcs" --cap "$CAP" --gi long --max_bw "$MAX_BW")
if [ $? -ne 0 ]; then
    echo "Error: bitrate_calculator.sh failed. Setting fallback, please retry with a lower bitrate."
    set_bitrate.sh 3000 0 --max_bw 20
    exit 1
fi

# RESULT is expected in the format: <mcs>:<bw>:<gi>:<fec>
candidate_mcs=$(echo "$RESULT" | cut -d: -f1)
candidate_bw=$(echo "$RESULT" | cut -d: -f2)
candidate_gi=$(echo "$RESULT" | cut -d: -f3)
candidate_fec=$(echo "$RESULT" | cut -d: -f4)

# --- Adjust candidate if its bandwidth exceeds allowed maximum ---
if [ "$candidate_bw" -gt "$max_bw_allowed" ]; then
    echo "Warning: Candidate BW ${candidate_bw} MHz exceeds allowed maximum (${max_bw_allowed} MHz)."
    candidate_bw="$max_bw_allowed"
fi

echo "Setting new configuration:"
echo "   Target bitrate: ${TARGET} kbps"
echo "   Selected settings: MCS=${candidate_mcs}, BW=${candidate_bw} MHz, GI=${candidate_gi}, FEC=${candidate_fec}"
echo "   Bitrate cap: ${CAP} kbps"
echo "   Allowed max BW: ${max_bw_allowed} MHz"
echo "   Update direction: ${DIRECTION}"
echo "   Transmitter Power: ${TX_PWR}"


# --- Execute update blocks based on --direction ---
if [ "$DIRECTION" = "decreased" ]; then
    # If "decreased", update Majestic Online first
    ## --- Update Majestic Online ---
    curl -s localhost/api/v1/set?video0.bitrate=$TARGET 2> /dev/null
    sleep 1
    
    
    # Then update WFB_NG Online
    # --- Update WFB_NG Online ---
    wfb_tx_cmd 8000 set_radio -B ${candidate_bw} -G ${candidate_gi} \
      -S $(yaml-cli -i /etc/wfb.yaml -g .broadcast.stbc) \
      -L $(yaml-cli -i /etc/wfb.yaml -g .broadcast.ldpc_tx) \
      -M ${candidate_mcs}
    wfb_tx_cmd 8000 set_fec -k ${fec_k} -n ${fec_n}
    
    # --- Set transmitter power ---
    echo "Setting transmitter power to ${TX_PWR}."
    set_alink_tx_pwr.sh "$TX_PWR" --mcs $candidate_mcs
    
else

    # --- Set transmitter power ---
    echo "Setting transmitter power to ${TX_PWR}."
    set_alink_tx_pwr.sh "$TX_PWR" --mcs $candidate_mcs
    
    # Otherwise, update WFB_NG Online first
    # --- Update WFB_NG Online ---
    wfb_tx_cmd 8000 set_radio -B ${candidate_bw} -G ${candidate_gi} \
      -S $(yaml-cli -i /etc/wfb.yaml -g .broadcast.stbc) \
      -L $(yaml-cli -i /etc/wfb.yaml -g .broadcast.ldpc_tx) \
      -M ${candidate_mcs}
    wfb_tx_cmd 8000 set_fec -k ${fec_k} -n ${fec_n}
    sleep 0.2
    ## --- Update Majestic Online ---
    curl -s localhost/api/v1/set?video0.bitrate=$TARGET 2> /dev/null
fi

echo "Bitrate settings applied successfully."
