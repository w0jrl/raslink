#!/bin/bash
# patch-asterisk.sh - Add customizations to Asterisk
#    Copyright (C) 2019  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Script Start
cd ../../astsrc/asterisk/
echo "Patching Asterisk files..."
# Add the notch option
cp ../extras/notch/rpt_notch.c ./apps
sed -i 's/\/\* #include "rpt_notch.c" \*\//#include "rpt_notch.c"/' ./apps/app_rpt.c
# Add mdc1200 support
cp ../extras/mdc1200/*.c ./apps
cp ../extras/mdc1200/*.h ./apps
sed -i 's/\/\* #include "mdc_decode.c" \*\//#include "mdc_decode.c"/' ./apps/app_rpt.c
sed -i 's/\/\* #include "mdc_encode.c" \*\//#include "mdc_encode.c"/' ./apps/app_rpt.c
# Update app_rpt version information
sed -i '/ \*  version/c\ \*  version 18.12 12\/01\/2019' ./apps/app_rpt.c
sed -i '/static  char \*tdesc \= "Radio Repeater \/ Remote Base  version/c\static  char \*tdesc \= "Radio Repeater \/ Remote Base  version 18.12 12\/01\/2019";' ./apps/app_rpt.c
# Change TX enabled message
sed -i 's/"RPTENA"/"TXENA"/' ./apps/app_rpt.c
# Make EchoLink call signs use normal characters
sed -i 's/return(sayphoneticstr/return\(saycharstr/' ./apps/app_rpt.c
sed -i 's/res \= sayphoneticstr/res \= saycharstr/' ./apps/app_rpt.c
# Prevent reading false DTMF digits
sed -i 's/26.0 : 42.0/32.0 : 42.0/' ./main/dsp.c
# Set queue size in usbradio driver
sed -i 's/define	QUEUE_SIZE	2/define	QUEUE_SIZE	3/' ./channels/chan_usbradio.c
#Update include statements in usbradio driver
dirent=$(grep -ic '#include <dirent.h>' ./channels/chan_usbradio.c)
libgen=$(grep -ic '#include <libgen.h>' ./channels/chan_usbradio.c)
if [ "$dirent" = "0" ] && [ "$libgen" = "0" ]; then
  sed -i 's/ && !defined(AST_BUILDOPT_LIMEY)//g' ./channels/chan_usbradio.c
  sed -i '/#include <search.h>/c\
#include <search.h>\
#include <dirent.h>\
#include <libgen.h>' ./channels/chan_usbradio.c
fi
# Make sure asterisk uses pthread
sed -i 's/ASTCFLAGS+=-lpthread -Wno-unused-result/ASTCFLAGS\+\=-pthread -Wno-unused-result/' ./Makefile
echo "Done"
exit 0
