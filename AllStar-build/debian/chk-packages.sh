#!/bin/bash
# chk-packages.sh - Check package status, and install needed packages
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
echo "Removing unneeded packages and data"
ntp=$(which ntpd)
echo "Checking NTP..."
if [ -e "${ntp}" ]; then
    echo "Removing NTP; No longer needed for AllStar."
    apt-get -qq autoremove --purge -y ntp
    echo "Cleaning the database"
    (apt-get clean;apt-get autoclean) &>/dev/null
else
    echo "NTP isn't installed; Skipping."
fi
subversion=/usr/bin/svn
echo "Checking Subversion..."
if [ -e "$subversion" ]; then
    echo "Removing Subversion; No longer needed for AllStar."
    apt-get -qq autoremove --purge -y subversion
    rm -rf /root/.subversion &>/dev/null
    echo "Cleaning the database"
    (apt-get clean;apt-get autoclean) &>/dev/null
else
    echo "Subversion isn't installed; Skipping."
fi
if [ -f /etc/motd ]; then
    find /etc/ -name "*motd*" -exec rm -rf {} +
fi
echo -e "Done\n"
echo "Checking and installing required packages..."
sourcesList=$( grep -ic "#deb-src" /etc/apt/sources.list )
if [ "$sourcesList" == "1" ]; then
    sed -i 's/#deb-src/deb-src/' /etc/apt/sources.list
    apt-get -qq update
fi
chmod +x /usr/src/utils/AllStar-build/common/required-tools.sh
/usr/src/utils/AllStar-build/common/required-tools.sh
echo -e "Done\n"
echo "checking Logrotate status..."
fstab=$(grep -ic "/var/log tmpfs defaults,noatime,nosuid,mode=0755,size=64m 0 0" /etc/fstab )
if [ "$fstab" == "1" ]; then
    echo "Old fstab log setting detected; Removing."
    sed -i '/tmpfs \/var\/log tmpfs defaults,noatime,nosuid,mode=0755,size=64m 0 0/d' /etc/fstab
    echo "Done"
fi
if [[ -e /etc/logrotate.d/asterisk ]] && [[ "$(grep -ic "compress" /etc/logrotate.d/asterisk)" == "1" ]]; then
    echo -e "Logrotate parameters are already up to date; Skipping.\n"
else
    echo "updating Logrotate parameters..."
    sed -i '/tmpfs \/var\/log tmpfs defaults,noatime,nosuid,mode=0755,size=100m 0 0/d' /etc/fstab
    if [ -e /etc/logrotate.d/asterisk ]; then
        rm -rf /etc/logrotate.d/asterisk
    fi
    chmod +x /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
    /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
    echo "Logs will be rotated once a month."
    echo -e "Done\n"
fi
exit 0
