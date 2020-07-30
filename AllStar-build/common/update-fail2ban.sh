#!/bin/bash
# update-fail2ban.sh -- Build Fail2Ban for RasLink
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
        echo -e "***FAIL2BAN FAILED TO INSTALL***\nPlease see <https://jlappliedtechnologies.com/raslink/> for assistance."
        exit 1
    fi
}
clone() {
    cd /usr/src/utils/astsrc/
    git clone https://github.com/fail2ban/fail2ban.git &>/dev/null
}
update() {
    cd /usr/src/utils/astsrc/fail2ban/
    (git clean -f;git checkout -f) &>/dev/null
    git pull &>/dev/null
}
# Get the sources
    # If they don't exist, clone them
if [ ! -e /usr/src/utils/astsrc/fail2ban/.git ]; then
    echo "Downloading Fail2Ban sources..."
    status clone
    echo "Done"
else
    # If they do exist, pull the latest version
    echo "Updating Fail2Ban sources..."
    status update
    echo "Done"
fi
sleep 0.5s
# Install from the fetched sources
cd /usr/src/utils/astsrc/fail2ban/
echo "Building and installing Fail2Ban..."
status ./setup.py install
echo "Done"
sleep 0.5s
# If no configuration files are available, install them
if [ ! -e /etc/fail2ban/fail2ban.local ] || [ ! -e /etc/fail2ban/jail.local ]; then
    echo "Installing Fail2Ban configuration..."
    status cp /usr/src/utils/AllStar-build/common/fail2ban-files/*.local /etc/fail2ban/
    status cp /usr/src/utils/AllStar-build/common/fail2ban-files/fail2ban.rotate /etc/logrotate.d/fail2ban
    status touch /var/log/fail2ban.log
    status cp /usr/src/utils/AllStar-build/common/fail2ban-files/fail2ban.service /etc/systemd/system/
    echo "Done"
else
    echo "Checking Fail2Ban configuration..."
    echo "Your Fail2Ban configuration is up-to-date."
    echo "Done"
fi
exit 0
