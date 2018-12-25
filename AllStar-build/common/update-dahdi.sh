#! /bin/bash
# update-dahdi.sh - Build Dahdi for AllStar
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
status() {
    $@
    if [ $? -ne 0 ]; then
        echo "***Dahdi failed to install***
Please see <https://jlappliedtechnologies.com/raslink/> for assistance."
        sleep 5
        exit 1
    fi
}
echo "Downloading and unpacking Dahdi..."
cd /usr/src/utils/
if [ -e /usr/src/utils/astsrc/dahdi* ]; then
    status rm -rf /usr/src/utils/astsrc/dahdi*
fi
status wget http://downloads.asterisk.org/pub/telephony/dahdi-linux-complete/dahdi-linux-complete-current.tar.gz &>/dev/null
cd /usr/src/utils/astsrc/
status tar zxvf /usr/src/utils/dahdi-linux-complete-current.tar.gz &>/dev/null
status mv dahdi* dahdi
status rm -rf /usr/src/utils/*.tar.gz
echo "Done"
cd /usr/src/utils/astsrc/dahdi/
echo "Building Dahdi..."
status make all
status make install
if [ ! -e /dahdi/system.conf ]; then
    /usr/sbin/dahdi_genconf &>/dev/null
    rm /etc/dahdi/*.bak &>/dev/null
fi
if [ "$(grep -ic "dahdi" /etc/modules)" == "1" ]; then
    sed -i '/dahdi/d' /etc/modules
fi
if [ -f /etc/init.d/dahdi ]; then
    update-rc.d dahdi remove
    rm -rf /etc/init.d/dahdi
    systemctl disable dahdi.timer
    rm -rf /etc/systemd/system/dahdi.timer
    systemctl daemon-reload
fi
echo "Done"
exit 0
