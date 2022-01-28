#!/bin/bash
# release-upgrade: Upgrade RasLink servers to the next stable Debian/Raspbian version
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
# Make sure release-upgrade runs in tmux
if [ -z "$TMUX" ]; then exec tmux new -s release-upgrade /bin/bash "$0"; fi
# Functions
status() {
    "$@"
    if [ $? -ne 0 ]; then
        echo -e "***RELEASE-UPGRADE FAILED***\nPlease see <https://jlappliedtechnologies.com/raslink/> for assistance."
        exit 1
    fi
}
prompt() {
    echo "${name} ${release} is available."
    read -e -p "Do you wish to upgrade? [Y/N]" upgrade
    if [[ "$upgrade" = "y" ]] || [[ "$upgrade" = "Y" ]] || [[ "$upgrade" = "" ]]; then
        echo "Upgrading to ${name} ${release}
This is going to take awhile.
Your node will remain disabled until 'system-update' completes.
During the upgrade, several questions will be asked.
If you're not sure about a question, choose the default
answer by pressing enter.
DO NOT POWER OFF THE SYSTEM!
PRESS ENTER TO CONTINUE"
        read
    else
        echo "Not upgrading to ${name} ${release}.
Run 'release-upgrade' when you are ready."
        echo "PRESS ENTER TO CONTINUE with normal 'system-update'"
        read
        exit 0
    fi
}
distro() {
    platform=$(lsb_release -is)
    name=RasLink
    prev=jessie
    release=stretch
}
check() {
    if [[ $(grep -ic "${prev}" /etc/apt/sources.list) = "0" ]]; then
        echo "Already updated to ${name} ${release}"
        exit 0
    fi
}
update() {
    sed -i "s/${prev}/${release}/" /etc/apt/sources.list /etc/apt/sources.list.d/*.list
    if [[ $platform = "Raspbian" ]]; then
        rm -rf /tmp/* /tmp/.* &>/dev/null
        rm -rf /var/tmp/* /var/tmp/.* &>/dev/null
        status umount /tmp &>/dev/null
        status umount /var/tmp &>/dev/null
        (status cp /usr/src/utils/RasLink-build/rpi/tmpfs.sh /usr/local/bin/tmpfs.sh;chmod +x /usr/local/bin/tmpfs.sh)
        status /usr/local/bin/tmpfs.sh
    fi
    status apt-get -qq autoremove --purge apt-listchanges gcc-4.8 gcc-4.9 -y
    (status apt-get update;apt-get upgrade -y;apt-get clean;apt-get autoclean)
    (status apt-get dist-upgrade -y;apt-get autoremove --purge -y;apt-get clean;apt-get autoclean)
    (status apt-get -qq install -y ssh;apt-get -qq update)
    if [[ $platform = "Raspbian" ]]; then
        (status apt-get -qq install --reinstall wpasupplicant -y;apt-get -qq clean;dpkg-reconfigure wpasupplicant)
    fi
    (status apt-get -qq purge -y $(dpkg -l | awk '/^rc/ { print $2 }'))
    hash -r
    sed -i '0,/^.PermitRootLogin/s/\(^.PermitRoot.*\)\|\(^PermitRoot.*\)/PermitRootLogin yes/' /etc/ssh/sshd_config
    if [ -d /etc/php5 ]; then
        rm -rf /etc/php5
    fi
    cd /root
    mv .bashrc .bashrc.orig
    cat .bashrc.orig > .bashrc
    if [[ $platform = "Raspbian" ]]; then
        echo "/usr/src/utils/RasLink-build/rpi/update-stage2.sh" >> .bashrc
    else
        echo "/usr/src/utils/RasLink-build/debian/update-stage2.sh" >> .bashrc
    fi
    echo "REBOOT TO FINISH UPGRADE"
    echo "When your node reboots, you need to log in
to finish the upgrade."
    echo "PRESS ENTER TO REBOOT"
    read
    sync
    reboot
}
# Run the upgrade
  distro
  check
  prompt
  update
exit 0
