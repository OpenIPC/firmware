#!/bin/sh

case "$1" in
    5)
        echo "Channel5 change."
        channel5.sh "$2"
        ;;
    6)
        echo "Channel6 change."
        channel6.sh "$2"
        ;;
    7)
        echo "Channel7 change."
        channel7.sh "$2"
        ;;
    8)
        echo "Channel8 change."
        channel8.sh "$2"
        ;;
    9)
        echo "Channel9 change."
        channel9.sh "$2"
        ;;
    10)
        echo "Channel10 change."
        channel10.sh "$2"
        ;;
    11)
        echo "Channel11 change."
        channel11.sh "$2"
        ;;
    12)
        echo "Channel12 change."
        channel12.sh "$2"
        ;;
    *)
        echo "No valid channel specified."
        ;;
esac

exit 0
