#!/bin/sh

#GPIO3_3 -> GPIO27 (3*8+3 = 27)
#GPIO3_4 -> GPIO28 (3*8+4 = 28)

#(normal mode)
ir_cut_enable()
{
	# pin_mux export
	echo "27" > /sys/class/gpio/export; 	# GPIO3_3
	echo "28" > /sys/class/gpio/export;  	# GPIO3_4

	# dir
	echo "out" > /sys/class/gpio/gpio27/direction;
	echo "out" > /sys/class/gpio/gpio28/direction;

	# data, GPIO3_4: 0, GPIO3_3: 1  (normal mode)
	echo "1" > /sys/class/gpio/gpio27/value;
	echo "0" > /sys/class/gpio/gpio28/value;

	#sleep 1s
	sleep 1;

	# back to original
	echo "0" > /sys/class/gpio/gpio27/value;
	echo "0" > /sys/class/gpio/gpio28/value;

	# pin_mux unexport
	echo "27" > /sys/class/gpio/unexport; 	# GPIO3_3
	echo "28" > /sys/class/gpio/unexport;  	# GPIO3_4
}

# (ir mode)
ir_cut_disable()
{
	# pin_mux export
	echo "27" > /sys/class/gpio/export;     # GPIO3_3
	echo "28" > /sys/class/gpio/export;  	# GPIO3_4

	# dir
	echo "out" > /sys/class/gpio/gpio27/direction;
	echo "out" > /sys/class/gpio/gpio28/direction;

	# data, GPIO3_4: 1, GPIO3_3: 0  (ir mode)
	echo "0" > /sys/class/gpio/gpio27/value;
	echo "1" > /sys/class/gpio/gpio28/value;

	#sleep 1s
	sleep 1;

	# back to original
	echo "0" > /sys/class/gpio/gpio27/value;
	echo "0" > /sys/class/gpio/gpio28/value;

	# pin_mux unexport
	echo "27" > /sys/class/gpio/unexport; 	# GPIO3_3
	echo "28" > /sys/class/gpio/unexport;  	# GPIO3_4
}

if [ $# -eq 0 ]; then
    echo "ir mode : ./demo_config.sh 1";
else
    if [ $1 -eq 0 ]; then
        echo "normal mode, ir_cut on"
        ir_cut_enable > /dev/null;
    fi

    if [ $1 -eq 1 ]; then
        echo "ir mode, ir_cut off"
        ir_cut_disable > /dev/null;
    fi
fi
