#!/bin/bash
# update-fail2ban.sh -- Build Fail2Ban for RasLink
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
        echo -e "***FAIL2BAN FAILED TO INSTALL***\nPlease see <https://jlappliedtechnologies.com/raslink/> for assistance."
        exit 1
    fi
}
clone() {
    cd /usr/src/utils/src/
    git clone https://github.com/fail2ban/fail2ban.git &>/dev/null
}
update() {
    cd /usr/src/utils/src/fail2ban/
    (git clean -f;git checkout -f) &>/dev/null
    git pull &>/dev/null
}
# Get the sources
    # If they don't exist, clone them
if [ ! -e /usr/src/utils/src/fail2ban/.git ]; then
    echo "Downloading Fail2Ban sources..."
    status clone
    echo -e "Done\n"
else
    # If they do exist, pull the latest version
    echo "Updating Fail2Ban sources..."
    status update
    echo -e "Done\n"
fi
sleep 0.5s
# Install from the fetched sources
cd /usr/src/utils/src/fail2ban/
echo "Building and installing Fail2Ban..."
status ./setup.py install
echo -e "Done\n"
sleep 0.5s
# If no configuration files are available, install them
if [ ! -e /etc/fail2ban/fail2ban.local ] || [ ! -e /etc/fail2ban/jail.d/00raslink.local ]; then
    echo "Installing Fail2Ban configuration..."
    (status cp /usr/src/utils/RasLink-build/common/fail2ban-files/*.local /etc/fail2ban/;status mv /etc/fail2ban/00raslink.local /etc/fail2ban/jail.d/)
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/fail2ban.rotate /etc/logrotate.d/fail2ban
    status touch /var/log/fail2ban.log
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/fail2ban.service /etc/systemd/system/
    echo -e "Done\n"
    isF2bRunning=$(ps aux | grep 'fail2ban-server' | wc -l)
    if [ "$isF2bRunning" != "1" ]; then
        echo "Loading Fail2Ban rules..."
        $(which fail2ban-client) reload
    else
        echo "Starting Fail2Ban..."
        $(which fail2ban-client) start
    fi
    echo  ""
else
    echo "Updating RasLink Fail2Ban rules if required..."
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/00raslink.local /etc/fail2ban/jail.d/ 
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/fail2ban.local /etc/fail2ban/ 
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/fail2ban.service /etc/systemd/system/
    echo -e "Done\n"
    echo "Reloading Fail2Ban rules..."
    $(which fail2ban-client) reload
    echo ""
fi
exit 0
