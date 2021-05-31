#!/bin/bash
# patch-asterisk.sh: Add customizations to Asterisk
#    Copyright (C) 2021  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Script Start
cd ../../src/asterisk/
echo "Patching Asterisk files..."
#
# Set on demand telemetry timer
sed -i '/#define\tTELEM_HANG_TIME 120000/c\#define\tTELEM_HANG_TIME 15000' ./apps/app_rpt.c
# Update app_rpt version information
sed -i '/ \*  version/c\ \*  version 21.06 06\/01\/2021' ./apps/app_rpt.c
sed -i '/static  char \*tdesc \= "Radio Repeater \/ Remote Base  version/c\static  char \*tdesc \= "Radio Repeater \/ Remote Base  version 21.06 06\/01\/2021";' ./apps/app_rpt.c
sed -i '/ASTERISK_FILE_VERSION(__FILE__, \"\$Revision/c\ASTERISK_FILE_VERSION\(__FILE__\, \"\$Revision 210601\"\)' ./apps/app_rpt.c
#sed -i '/note ported by Michael Zingman, N4IRR/a\
# \* \\note modified by Jeremy Lincicome, W0JRL' ./apps/app_rpt.c
# Change TX enabled message
sed -i 's/"RPTENA"/"TXENA"/' ./apps/app_rpt.c
# Make EchoLink call signs use normal characters
sed -i 's/return(sayphoneticstr/return\(saycharstr/' ./apps/app_rpt.c
sed -i 's/res \= sayphoneticstr/res \= saycharstr/' ./apps/app_rpt.c
# Prevent reading false DTMF digits
sed -i 's/26.0 : 42.0/32.0 : 42.0/' ./main/dsp.c
# Set queue size in usbradio driver
sed -i 's/define	QUEUE_SIZE	2/define	QUEUE_SIZE	4/' ./channels/chan_usbradio.c
# Include pthread.h in sources
if [ "$(grep -ic 'pthread.h' ./channels/chan_usbradio.c)" = "0" ];then
    sed -i '/stdio.h/i\
#include <pthread.h>' ./channels/chan_usbradio.c
fi
if [ "$(grep -ic 'pthread.h' ./channels/chan_simpleusb.c)" = "0" ];then
    sed -i '/stdio.h/i\
#include <pthread.h>' ./channels/chan_simpleusb.c
fi
if [ "$(grep -ic 'pthread.h' ./apps/app_rpt.c)" = "0" ];then
    sed -i '/signal.h/i\
#include <pthread.h>' ./apps/app_rpt.c
fi
# Make sure asterisk core uses pthread
sed -i 's/ASTCFLAGS+=-lpthread -Wno-unused-result/ASTCFLAGS\+\=-pthread -Wno-unused-result/' ./Makefile
# Set libcurl timeout
sed -i 's/CURLOPT_TIMEOUT\, 30/CURLOPT_TIMEOUT\, 2/' ./apps/app_rpt.c
echo "Done"
exit 0
