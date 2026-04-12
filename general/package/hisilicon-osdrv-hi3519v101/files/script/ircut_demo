#!/bin/sh

#GPIO6_5 -> GPIO53 (6*8+5 = 53)
#GPIO6_6 -> GPIO54 (6*8+6 = 54)
#GPIO3_0 -> GPIO24 (3*8+0 = 24)

#(normal mode)
ir_cut_enable()
{
	# pin_mux
	echo "$gpio_0" > /sys/class/gpio/unexport;
	echo "$gpio_1" > /sys/class/gpio/unexport;
	echo "$gpio_0" > /sys/class/gpio/export;
	echo "$gpio_1" > /sys/class/gpio/export;
	
	# dir
	echo "out" > /sys/class/gpio/gpio$gpio_0/direction;
	echo "out" > /sys/class/gpio/gpio$gpio_1/direction;
	
	# data, gpio_1: 0, gpio_0: 1  (normal mode)
	echo "1" > /sys/class/gpio/gpio$gpio_0/value;
	echo "0" > /sys/class/gpio/gpio$gpio_1/value;
	
	#sleep 1s
	sleep 1;
	
	# back to original 
	echo "0" > /sys/class/gpio/gpio$gpio_0/value;
	echo "0" > /sys/class/gpio/gpio$gpio_1/value;
}

# (ir mode)
ir_cut_disable()
{
	# pin_mux
	echo "$gpio_0" > /sys/class/gpio/unexport;
	echo "$gpio_1" > /sys/class/gpio/unexport;
	echo "$gpio_0" > /sys/class/gpio/export;
	echo "$gpio_1" > /sys/class/gpio/export;
	
	# dir
	echo "out" > /sys/class/gpio/gpio$gpio_0/direction;
	echo "out" > /sys/class/gpio/gpio$gpio_1/direction;
	
	# data, gpio_1: 1, gpio_0: 0  (ir mode)
	echo "0" > /sys/class/gpio/gpio$gpio_0/value;
	echo "1" > /sys/class/gpio/gpio$gpio_1/value;
	
	#sleep 1s
	sleep 1;
	
	# back to original 
	echo "0" > /sys/class/gpio/gpio$gpio_0/value;
	echo "0" > /sys/class/gpio/gpio$gpio_1/value;
}

gpio_0=0
gpio_1=0

if [ $# -lt 2 ]; then
    echo "usage : ./demo_config.sh <chip> <mode>";
    echo "for example:";
    echo "ir mode : ./demo_config.sh hi3516cv300 1";
else
	if [ $1 = "hi3516ev100" ]; then
		gpio_0=53;
		gpio_1=24;
	elif [ $1 = "hi3516cv300" ]; then
		gpio_0=53;
		gpio_1=54;
	else
		echo "wrong chipid: $1, please select: hi3516cv300 or hi3516ev100.";
		exit;
	fi

    if [ $2 -eq 0 ]; then
        echo "normal mode, ir_cut on"
        ir_cut_enable > /dev/null;
    elif [ $2 -eq 1 ]; then
        echo "ir mode, ir_cut off"
        ir_cut_disable > /dev/null;
	else
		echo "invalid mode, please slect 0 or 1."
    fi
fi
