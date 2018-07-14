#!/bin/bash
# update-stage1.sh - Update the system
# Stage One
#    Copyright (C) 2018  Jeremy Lincicome (W0JRL)
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
# Make sure system-update runs in screen
if [ -z "$STY" ]; then exec screen -S system-update /bin/bash "$0"; fi
echo "Starting system update.
This will take a while.
System-update is running in a screen session.
If your session disconnects during the update,
after reconnecting, run
'screen -dr'
to reconnect to the update screen.
You can continue using your node during this process."
sleep 3
# Get new sources
echo "Updating source files for All Star..."
cd /usr/src/utils/
git pull >&2>/dev/null
if [ $? -ne 0 ]; then
    echo "Failed to download update.
Aborting system update.
Please see <https://jlappliedtechnologies.com/raslink/> for assistance."
    sleep 5
    exit 1
fi
sleep 0.5
echo "Done"
# Update the system
echo "Updating system software..."
(apt-get update;apt-get dist-upgrade -y)
sleep 0.5
echo "Done"
# Clean the package database
echo "Cleaning up unneeded software..."
(apt-get -qq autoremove --purge -y;apt-get -qq clean;apt-get -qq autoclean)
apt-get -qq purge -y $(dpkg -l | awk '/^rc/ { print $2 }')
sleep 0.5
echo "Done"
# Setup for stage two
cd /root
mv .bashrc .bashrc.orig
cat .bashrc.orig > .bashrc
echo "/usr/src/utils/AllStar-build/rpi/update-stage2.sh" >> .bashrc
systemctl disable asterisk.timer &>/dev/null
systemctl disable asterisk.service &>/dev/null
echo "Rebooting to finish install"
echo "When your node reboots, you need to log in
to finish the update."
sync
sudo reboot
exit 0
