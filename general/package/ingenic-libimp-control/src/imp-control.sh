#!/bin/sh

# This script provides a control interface for the libimp_control library.
# It sends commands to the server listening on localhost port 4000.

full_demo() {

demo() {
    echo "imp_control $*" | nc localhost 4000 >/dev/null 2>&1
}

random_range() {
    echo $(($1 + RANDOM % ($2 - $1 + 1)))
}

run_demo() {
    echo "Running $1..."
    for v in $(seq 127 -1 1) $(seq 1 255) $(seq 254 -1 127); do
        echo -n "$v."
        demo $1 $v
        sleep 0.004
    done
    echo -e "\n$1 Done"
}

mask() {
    start=$(date +%s)
    echo "Mask demo"
    while [ $(($(date +%s) - start)) -lt 10 ]; do
        demo mask "0 1 $(random_range 0 640) $(random_range 0 1024) 320 240 $(random_range 0 255) $(random_range 0 255) $(random_range 0 255)"
        sleep 0.5
    done
    demo mask 0 0 516 34 320 240 49 98 163
}
mask

echo "Whitebalance demo"
for wb in $(seq 1 9) 0; do demo whitebalance $wb; sleep 1; done
echo "isp mode demo"
demo ispmode 1; sleep 3; demo ispmode 0; sleep 1

echo "Front Crop Demo"
for v in $(seq 0 400); do demo frontcrop 1 $v 0 1280 720; done
for v in $(seq 0 800); do demo frontcrop 1 0 $v 1280 720; done
demo frontcrop 1 0 0 1920 1080

echo "autozoom demo"
for zoom in 1.05 1.10 1.15 1.20 1.15 1.10 1.05; do
    demo autozoom 0 1 $(awk "BEGIN {print int(1920 * $zoom)}") $(awk "BEGIN {print int(1080 * $zoom)}") 1 0 0 1920 1080
    echo "Autozoom: $zoom"
    sleep 1
done
demo autozoom 0 0 2304 1296 0 0 0 1920 1080

for func in brightness contrast saturation sharpness sinter temper aecomp dpc drc hue; do run_demo $func; done
}

if [ "$1" == "full_demo" ]; then
    full_demo
else
    # Send the command and parameters to the server
    echo "imp_control $*" | nc localhost 4000
fi
