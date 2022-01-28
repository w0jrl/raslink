#!/bin/bash
# chk-packages.sh: Check package status, and remove packages and configuration files that aren't needed.
#   Copyright (C) 2016-2022  Jeremy Lincicome W0JRL
#   <https://jlappliedtechnologies.com>  <admin@jlappliedtechnologies.com>
#
#   This file is part of RasLink.
#   RasLink is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   RasLink is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with RasLink.  If not, see <http://www.gnu.org/licenses/>.
#
# Script Start
echo "Removing unneeded packages and data"
ntp=$(which ntpd)
echo "Checking NTP..."
if [ -e "${ntp}" ]; then
    echo "Removing NTP; No longer needed for RasLink."
    apt-get -qq autoremove --purge -y ntp
    echo "Cleaning the database"
    (apt-get clean;apt-get autoclean) &>/dev/null
else
    echo "NTP isn't installed; Skipping."
fi
echo "Looking for Wolfram Engine..."
if [ -e /usr/bin/wolfram ]; then
    echo "Uninstalling Wolfram Engine; Not needed for RasLink."
    apt-get -qq purge -y wolfram-engine &>/dev/null
    echo "Cleaning the database"
    (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean) &>/dev/null
    echo "Removing Wolfram Engine data"
    rm -rf /opt/Wolfram &>/dev/null
else
    echo "Wolfram Engine isn't installed; Skipping."
fi
echo "Looking for Minecraft..."
if [ -e /usr/bin/minecraft-pi ]; then
    echo "Uninstalling Minecraft; Not needed for RasLink."
    apt-get -qq purge -y minecraft-pi &>/dev/null
    echo "Cleaning the database"
    (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean) &>/dev/null
else
    echo "Minecraft isn't installed; Skipping."
fi
chmod +x /usr/src/utils/RasLink-build/rpi/rm-pi.sh
/usr/src/utils/RasLink-build/rpi/rm-pi.sh
echo "Looking for RPI Update..."
if [ -e /usr/bin/rpi-update ]; then
    echo "Uninstalling RPI Update; No longer needed for RasLink."
    apt-get purge -y rpi-update &>/dev/null
    echo "Cleaning the database"
    (apt-get autoremove --purge -y;apt-get clean;apt-get autoclean) &>/dev/null
else
    echo "RPI Update isn't installed; Skipping."
fi
subversion=/usr/bin/svn
echo "Checking Subversion..."
if [ -e "${subversion}" ]; then
    echo "Removing Subversion; No longer needed for RasLink."
    apt-get -qq autoremove --purge -y subversion
    rm -rf /root/.subversion &>/dev/null
    echo "Cleaning the database"
    (apt-get clean;apt-get autoclean) &>/dev/null
else
    echo "Subversion isn't installed; Skipping."
fi
if [ -f /etc/motd ] || [ -d /etc/update-motd.d ]; then
    find /etc/ -name "*motd*" -exec rm -rf {} +
fi
if [ -f /root/.nodeconverted ]; then
    rm /root/.nodeconverted
fi
if [ -f /etc/systemd/system/updatenodelist.service ]; then
    systemctl disable updatenodelist.service &>/dev/null
    systemctl stop updatenodelist.service &>/dev/null
    rm /etc/systemd/system/updatenodelist.service
fi
if [ -f /usr/local/bin/rc.updatenodelist ]; then
    rm /usr/local/bin/rc.updatenodelist
fi
if [ -f /usr/local/bin/zram.start ] || [ -f /usr/local/bin/zram.stop ]; then
    rm -rf /usr/local/bin/zram.s*
fi
if [ -f /usr/local/bin/zram.sh ]; then
    rm -f /usr/local/bin/zram.sh
fi
echo -e "Done\n"
echo "Checking and installing required packages..."
chmod +x /usr/src/utils/RasLink-build/common/required-tools.sh
/usr/src/utils/RasLink-build/common/required-tools.sh
echo -e "Done\n"
echo "checking Logrotate status..."
fstab=$(grep -ic "/var/log tmpfs defaults,noatime,nosuid,mode=0755,size=64m 0 0" /etc/fstab )
if [ "$fstab" == "1" ]; then
    echo "Old fstab log setting detected; Removing."
    sed -i '/tmpfs \/var\/log tmpfs defaults,noatime,nosuid,mode=0755,size=64m 0 0/d' /etc/fstab
    echo "Done"
fi
if [ -e /etc/logrotate.d/asterisk ] && [ "$(grep -icw 'compress' /etc/logrotate.d/asterisk)" == "1" ] && [ "$(grep -icw 'delaycompress' /etc/logrotate.d/asterisk)" == "1" ] && [ "$(grep -ic 'maxage' /etc/logrotate.d/asterisk)" == "1" ]; then
    echo -e "Logrotate parameters are already up to date; Skipping.\n"
else
    echo "updating Logrotate parameters..."
    sed -i '/tmpfs \/var\/log tmpfs defaults,noatime,nosuid,mode=0755,size=100m 0 0/d' /etc/fstab
    if [ -e /etc/logrotate.d/asterisk ]; then
        rm -rf /etc/logrotate.d/asterisk
    fi
    chmod +x /usr/src/utils/RasLink-build/common/mk-logrotate-asterisk.sh
    /usr/src/utils/RasLink-build/common/mk-logrotate-asterisk.sh
    echo "Logs will be rotated once a week."
    echo -e "Done\n"
fi
if [ "$( grep -ic "i2c-dev" /etc/modules )" == "0" ]; then
    echo "i2c-dev" >> /etc/modules
    modprobe i2c-dev
fi
exit 0
