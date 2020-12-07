#!/bin/bash
# update-dahdi.sh - Build Dahdi for AllStar
#    Copyright (C) 2021  Jeremy Lincicome (W0JRL)
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
        echo -e "***DAHDI FAILED TO INSTALL***\nPlease see <https://jlappliedtechnologies.com/raslink/> for assistance."
        exit 1
    fi
}
echo "Downloading and unpacking Dahdi..."
cd /usr/src/utils/
if [ -e /usr/src/utils/astsrc/dahdi ]; then
    status rm -rf /usr/src/utils/astsrc/dahdi
fi
status wget http://downloads.asterisk.org/pub/telephony/dahdi-linux-complete/dahdi-linux-complete-current.tar.gz &>/dev/null
cd /usr/src/utils/astsrc/
status tar zxvf /usr/src/utils/dahdi-linux-complete-current.tar.gz &>/dev/null
status mv dahdi-linux-* dahdi
echo -e "Done\n"
cd /usr/src/utils/astsrc/dahdi/
echo "Building and installing Dahdi..."
status patch -p1 < /usr/src/utils/astsrc/dahdi-patches/patch-dahdi-no-pciradio
status patch -p0 < /usr/src/utils/astsrc/dahdi-patches/patch-dahdi.rules
export MODULES_EXTRA='dahdi_dummy'
status make -j $(nproc) all
status make -j $(nproc) install
echo -e "Done\n"
echo "Updating Dahdi configuration..."
/sbin/modprobe dahdi_dummy &>/dev/null
/usr/sbin/dahdi_span_assignments auto &>/dev/null
/usr/sbin/dahdi_genconf &>/dev/null
status rm -f /etc/dahdi/*.bak /etc/asterisk/dahdi*.bak /usr/src/utils/1 /usr/src/utils/dahdi*.gz &>/dev/null
if [ "$(grep -ic '#include dahdi-channels.conf' /etc/asterisk/chan_dahdi.conf)" == "0" ]; then
    echo '#include dahdi-channels.conf' >> /etc/asterisk/chan_dahdi.conf
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
echo -e "Done\n"
exit 0
