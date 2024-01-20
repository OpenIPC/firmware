#!/bin/sh

# Get the timezone from the u-boot environment variable
timezone=$(fw_printenv -n timezone 2>/dev/null)
if [ -z "$timezone" ]; then
	echo "Timezone env variable not found, using system default."
	exit 1
fi

echo "User defined timezone: $timezone"

# Check if the values in /etc/timezone and /etc/TZ match the ones from fw_printenv
current_timezone=$(cat /etc/timezone 2>/dev/null)
current_tz_value=$(cat /etc/TZ 2>/dev/null)

if [ "$timezone" = "$current_timezone" ] && [ "$timezone" = "$current_tz_value" ]; then
	echo "Timezone settings are already up to date."
	exit 0
fi

# Search for the timezone in the file
matching_line=$(zcat /var/www/a/tz.js.gz | grep -i -F "$timezone")
if [ -z "$matching_line" ]; then
	echo "Timezone not found in system file."
	exit 1
fi

# Extract the value associated with the timezone
value=$(echo "$matching_line" | sed "s/^.*v:'\([^']*\)'.*$/\1/")

# Write the TZ file first
echo $value > /etc/TZ

# Then write the timezone file
echo $timezone > /etc/timezone

export TZ=$value

if tty -s; then
	echo "timezone.sh: You are running from a shell, please restart or log out to update timezone environment variables."
fi
