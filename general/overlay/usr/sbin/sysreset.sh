#!/bin/sh

show_help() {
	echo "Usage: $0 [OPTIONS]
  -m          Reset Majestic config.
  -n          Reset network config.
  -h          Show this help.
"
	exit 0
}

reset_majestic() {
	cp -f /rom/etc/majestic.yaml /etc/majestic.yaml
}

reset_network() {
	cp -f /rom/etc/network/interfaces /etc/network/interfaces
}

while getopts hmn flag; do
	case ${flag} in
		m) reset_majestic ;;
		n) reset_network ;;
		h) show_help ;;
	esac
done

exit 0
