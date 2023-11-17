#!/bin/sh

# This script provides a control interface for the libimp_control library.
# It sends commands to the server listening on localhost port 4000.

# Function to display the usage of the script
show_help() {
    echo "Usage: imp-control.sh [command] [parameters]"
    echo "       Use without parameters to retrieve current values."
    echo "Available commands:"
    echo "Command           Parameters                      Description"
    echo "-------           ----------                       -----------"
    echo "aihpf             [on/off]                         High Pass Filter"
    echo "aiagc             [off/gainLevel compGaindB]       Auto Gain Control"
    echo "ains              [off/0-3]                        Noise Suppression"
    echo "aiaec             [on/off]                         Echo Cancellation"
    echo "aivol             [-30-120]                        Audio Input Volume"
    echo "aigain            [0-31]                           Audio Input Gain"
    echo "aialc             [0-7]                            Audio Input ALC Gain"
    echo "aovol             [-30-120]                        Audio Output Volume"
    echo "aogain            [0-31]                           Audio Output Gain"
    echo "contrast          [0-255]                          Contrast"
    echo "brightness        [0-255]                          Brightness"
    echo "saturation        [0-255]                          Saturation"
    echo "sharpness         [0-255]                          Sharpness"
    echo "sinter            [0-255]                          Sinter Strength"
    echo "temper            [0-255]                          Temper Strength"
    echo "aecomp            [0-255]                          AE Compensation"
    echo "aeitmax           [value]                          AE Max parameters"
    echo "backlightcomp     [strength]                       Set Backlight Comp."
    echo "dpc               [0-255]                          DPC Strength"
    echo "drc               [0-255]                          DRC Strength"
    echo "hilight           [0-10]                           Highlight Intensity"
    echo "again             [value]                          Analog Gain"
    echo "dgain             [value]                          Digital Gain"
    echo "hue               [0-255]                          Hue"
    echo "ispmode           [0/1]                            ISP Mode (Day/Night)"
    echo "flicker           [0/1/2]                          Anti-Flicker"
    echo "flip              [normal/flip/mirror/flip_mirror] Image Flip"
    echo "fps               [fps_num] [fps_den]              Set Sensor FPS"
    echo "whitebalance      [mode] [rgain] [bgain]           Set White Balance"
    echo "gamma                                              Get Gamma Values"
    echo "autozoom          [ch] [scaler enable] [scale w] [scale h] [crop enable] [crop left] [crop top] [crop w] [crop h]"
    echo "                                                   Set Auto Zoom"
    echo "frontcrop         [enable] [top] [left] [width] [height]"
    echo "                                                   Set Front Crop"
    echo "mask              [ch] [enable] [top] [left] [width] [height] [green] [blue] [red]"
    echo "                                                   Set Mask"
}

# Check for no arguments and display help
if [ "$#" -eq 0 ]; then
    show_help
else
    # Send the command and parameters to the server
    echo "imp_control $*" | nc localhost 4000
fi
