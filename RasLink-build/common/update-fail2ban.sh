#!/bin/bash
# update-fail2ban.sh: Build Fail2Ban for RasLink
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
status() {
    "$@"
    if [ $? -ne 0 ]; then
        echo -e "***FAIL2BAN FAILED TO INSTALL***\nPlease see <https://jlappliedtechnologies.com/raslink/> for assistance."
        exit 1
    fi
}
isF2bRunning="$(command pgrep -x fail2ban-server > /dev/null)"
if [ ! "$isF2bRunning" ]; then
    echo "Stopping Fail2Ban..."
    command fail2ban-client stop
fi
clone() {
    cd /usr/src/utils/src/
    git clone https://github.com/fail2ban/fail2ban.git &>/dev/null &
}
update() {
    cd /usr/src/utils/src/fail2ban/
    (git clean -fdx&&git checkout -f) &>/dev/null &
    wait
    (git fetch --all --prune &&git pull) &>/dev/null &
}
# Get the sources
    # If they don't exist, clone them
if [ ! -e /usr/src/utils/src/fail2ban/.git ]; then
    echo "Downloading Fail2Ban sources..."
    status clone
    wait
    echo "Done"
else
    # If they do exist, pull the latest version
    echo "Updating Fail2Ban sources..."
    status update
    wait
    echo "Done"
fi
# Install from the fetched sources
cd /usr/src/utils/src/fail2ban/
echo "Building and installing Fail2Ban..."
if [ "$(command -v python3)" ]; then
    pycmd="python3"
    status git checkout master &>/dev/null
else
    pycmd="python2"
    status git checkout 0.9 &>/dev/null
fi
status ${pycmd} ./setup.py install
echo "Done"
# If no configuration files are available, install them
if [ ! -e /etc/fail2ban/fail2ban.local ] || [ ! -e /etc/fail2ban/jail.d/00raslink.local ]; then
    echo "Installing Fail2Ban configuration..."
    (status cp /usr/src/utils/RasLink-build/common/fail2ban-files/*.local /etc/fail2ban/;status mv /etc/fail2ban/00raslink.local /etc/fail2ban/jail.d/)
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/fail2ban.rotate /etc/logrotate.d/fail2ban
    status touch /var/log/fail2ban.log
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/fail2ban.service /etc/systemd/system/
    status command systemctl daemon-reload
    echo "Done"
else
    echo "Updating RasLink Fail2Ban rules if required..."
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/00raslink.local /etc/fail2ban/jail.d/ 
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/fail2ban.local /etc/fail2ban/ 
    status cp /usr/src/utils/RasLink-build/common/fail2ban-files/fail2ban.service /etc/systemd/system/
    status command systemctl daemon-reload
    sed -i '\|ignorecommand =|c\ignorecommand = %(fail2ban_confpath)s/filter.d/ignorecommands/apache-fakegooglebot <ip>' /etc/fail2ban/jail.local
    echo "Done"
fi
echo "Starting Fail2Ban..."
command fail2ban-client start
echo ""
exit 0
