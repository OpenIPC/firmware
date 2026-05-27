#!/bin/sh
# KC110 IR-LED + IR-cut master via sysfs gpio export.
# GPIO 77 = IR-LED (active-high, vendor UCI: ledOnValue=1, ledOffValue=0)
# GPIO 80 = IR-cut master (active-low, vendor: set_ir_cut(1) writes 0)
# Modes:
#   day   - LED off, IR-cut filter IN  (sees IR as colour-cast — bad, so cut blocks it)
#   night - LED on,  IR-cut filter OUT (lets IR illuminate the scene)

GPIO_LED=77
GPIO_CUT=80

export_pin() {
	[ -d /sys/class/gpio/gpio$1 ] || echo $1 > /sys/class/gpio/export 2>/dev/null
	echo out > /sys/class/gpio/gpio$1/direction 2>/dev/null
}

set_pin() {
	echo $2 > /sys/class/gpio/gpio$1/value
}

case "$1" in
	day)
		export_pin $GPIO_LED
		export_pin $GPIO_CUT
		set_pin $GPIO_LED 0
		set_pin $GPIO_CUT 0   # active-low: 0 = filter IN
		;;
	night)
		export_pin $GPIO_LED
		export_pin $GPIO_CUT
		set_pin $GPIO_CUT 1   # active-low: 1 = filter OUT
		sleep 1               # let mechanical filter settle
		set_pin $GPIO_LED 1
		;;
	off)
		set_pin $GPIO_LED 0 2>/dev/null
		set_pin $GPIO_CUT 0 2>/dev/null
		;;
	status)
		echo "led:    $(cat /sys/class/gpio/gpio$GPIO_LED/value 2>/dev/null || echo unset)"
		echo "ircut:  $(cat /sys/class/gpio/gpio$GPIO_CUT/value 2>/dev/null || echo unset)"
		;;
	*)
		echo "usage: $0 {day|night|off|status}" >&2
		exit 1
		;;
esac
