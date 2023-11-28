#!/bin/sh

# This script provides a control interface for the libimp_control library.
# It sends commands to the server listening on localhost port 4000.

# Send the command and parameters to the server
echo "imp_control $*" | nc localhost 4000
