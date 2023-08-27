#!/bin/sh

# Set the system timezone based on the u-boot environment variable

# Convert the timezone from lowercase, replace underscores with spaces for proper searching
convert_timezone() {
    local converted=""
    IFS="/"
    for part in $1; do
        part="$(echo "$part" | sed 's/_/ /g' | awk '{for(i=1;i<=NF;i++) $i=toupper(substr($i,1,1)) tolower(substr($i,2));}1' OFS=" " )"
        if [ -z "$converted" ]; then
            converted="$part"
        else
            converted="$converted/$part"
        fi
    done
    unset IFS
    echo "$converted"
}

# Get the timezone from the u-boot environment variable
timezone=$(fw_printenv -n timezone 2>/dev/null)
if [ -z "$timezone" ]; then
    echo "Timezone env variable not found, using system default."
    exit 1
fi

converted_timezone=$(convert_timezone "$timezone")
echo "User defined timezone: $converted_timezone"

# Check if the values in /etc/timezone and /etc/TZ match the ones from fw_printenv
current_timezone=$(cat /etc/timezone 2>/dev/null)
current_tz_value=$(cat /etc/TZ 2>/dev/null)

if [ "$converted_timezone" = "$current_timezone" ] && [ "$timezone" = "$current_tz_value" ]; then
    echo "Timezone settings are already up to date."
    exit 0
fi

# Search for the transformed timezone in the file
matching_line=$(zcat /var/www/a/tz.js.gz | grep -i -F "$converted_timezone")
if [ -z "$matching_line" ]; then
    echo "Timezone not found in system file."
    exit 1
fi

# Extract the value associated with the timezone
value=$(echo "$matching_line" | awk -F',' '{print $2}' | awk -F':' '{print $2}' | tr -d "'}")

# Write the TZ file first
echo $value > /etc/TZ

# Then write the timezone file
echo $converted_timezone > /etc/timezone

export TZ=$value

if tty -s; then
    echo "timezone.sh: You are running from a shell, please restart or log out to update timezone environment variables."
fi
