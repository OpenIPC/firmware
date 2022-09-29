#
# sigmon - 3G/GPRS datacard signal strength monitor
#
# $Id: sigmon,v 1.1 2006/10/20 14:19:54 pharscape Exp $
#

if [ $# -eq 1 ]
then
	if [ "$1" == '-?' ] 
	then
		echo "Syntax:   sigmon"
		echo "Function: 3G/GPRS datacard signal strength monitor"
		echo "Options:  none"
	else
		echo "Invalid option - $1"
	fi
	exit 1
else
	if [ $# -gt 1 ]
	then
		echo "Error: only one option permitted"
		exit 1
	fi
fi

echo "Use Ctrl/C to terminate monitoring"

while true
do
	comgt -d /dev/ttyUSB2 sig
	sleep 5
done
#
#   sigmon - use comgt to continuously monitor signal strength
#   Copyright (C) 2005  Martin Gregorie
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#    martin@gregorie.org
#

