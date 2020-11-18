#! /bin/bash
# update-asterisk.sh - Build asterisk for AllStar
#    Copyright (C) 2020  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com
#
#    This program is free software: you can redistribute it and/or modify
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
status() {
    "$@"
    if [ $? -ne 0 ]; then
        echo -e "***ASTERISK FAILED TO INSTALL***\nPlease see <https://jlappliedtechnologies.com/raslink/> for assistance."
        exit 1
    fi
}
echo "Building and installing Asterisk..."
# Remove modules
status rm -f /usr/lib/asterisk/modules/*
cd /usr/src/utils/astsrc/asterisk/
# Configure the build process
chmod +x ./bootstrap.sh
status ./bootstrap.sh
export PTLIB_CONFIG=/usr/share/ptlib/make/ptlib-config
status ./configure CC=gcc CPP=cpp
# Build and install Asterisk
status make
status make install
if [ -e /etc/init.d/asterisk ]; then
    (update-rc.d asterisk remove;rm /etc/init.d/asterisk)
fi
if [ -e /etc/systemd/system/asterisk.timer ]; then
    (systemctl disable asterisk.timer;rm /etc/systemd/system/asterisk.timer) &>/dev/null
fi
if [ -e /usr/local/bin/dsp.startup ]; then
    rm  /usr/local/bin/dsp.startup
fi
echo -e "Done\n"
chmod +x /usr/src/utils/AllStar-build/common/asterisk-config-check.sh
status /usr/src/utils/AllStar-build/common/asterisk-config-check.sh
exit 0
