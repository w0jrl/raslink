#!/bin/bash
# update-stage1.sh - Update the system
# Stage One
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
clear
# Make sure system-update runs in screen
if [ -z "$STY" ]; then exec screen -S system-update /bin/bash "$0"; fi
echo -e "STARTING SYSTEM UPDATE\nThis will take a while.\nSystem-update is running in a screen session.\nIf your session disconnects during the update,\nafter reconnecting, run\n'screen -dr'\nto reconnect to the update screen.\nYou can continue using your node during this process.\nPRESS ENTER TO CONTINUE"
read
# Get new sources
echo "Updating source files for All Star..."
cd /usr/src/utils/
git pull >&2>/dev/null
if [ "$?" -ne "0" ]; then
    echo -e "FAILED TO DOWNLOAD UPDATE\nABORTING SYSTEM UPDATE\nPlease see <https://jlappliedtechnologies.com/raslink/> for assistance."
    echo "PRESS ENTER TO EXIT"
    read
    exit 1
fi
echo -e "Done\n"
sleep 0.5
# Update the system
echo "Updating system software..."
(apt-get update;apt-get --fix-missing --fix-broken dist-upgrade -y)
echo -e "Done\n"
sleep 0.5
# Clean the package database
echo "Cleaning up unneeded software..."
(apt-get -qq autoremove --purge -y;apt-get -qq clean;apt-get -qq autoclean)
apt-get -qq purge -y $(dpkg -l | awk '/^rc/ { print $2 }')
echo -e "Done\n"
sleep 0.5
# Setup for stage two
cd /root
mv .bashrc .bashrc.orig
cat .bashrc.orig > .bashrc
echo "/usr/src/utils/AllStar-build/rpi/update-stage2.sh" >> .bashrc
systemctl disable asterisk.timer asterisk.service nodenames.service updatenodelist.service &>/dev/null
echo -e "REBOOT TO FINISH INSTALL\nWhen your node reboots, you need to log in\nto finish the update."
echo "PRESS ENTER TO REBOOT"
read
sync
reboot
exit 0
