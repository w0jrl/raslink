#!/bin/bash
# release-upgrade - Upgrade nodes to the next stable Debian/Raspbian version
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
# Make sure release-upgrade runs in screen
if [ -z "$STY" ]; then exec screen -S release-upgrade /bin/bash "$0"; fi
# Functions
prompt() {
    echo "${name} ${release} is available."
    read -e -p "Do you wish to upgrade? [Y/N]" upgrade
    if [[ "$upgrade" = "y" ]] || [[ "$upgrade" = "Y" ]]; then
        echo "Upgrading to ${name} ${release}
This is going to take awhile.
During the upgrade, several questions will be asked.
If you're not sure about a question, choose the default
answer by pressing enter.
DO NOT POWER OFF THE SYSTEM!"
        sleep 3s
    else
        echo "Not upgrading to ${name} ${release}.
Run 'release-upgrade' when you are ready."
sleep 2s
        exit 1
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
        sleep 3s
        exit 0
    fi
}
update() {
    sed -i "s/${prev}/${release}/" /etc/apt/sources.list /etc/apt/sources.list.d/*.list
    if [[ $platform = "Raspbian" ]]; then
        umount /tmp &>/dev/null
        umount /var/tmp &>/dev/null
        (cp /usr/src/utils/AllStar-build/rpi/tmpfs.sh /usr/local/bin/tmpfs.sh;chmod +x /usr/local/bin/tmpfs.sh)
        /usr/local/bin/tmpfs.sh
    fi
    apt-get -qq autoremove --purge apt-listchanges libpt-dev -y
    (apt-get update;apt-get upgrade -y;apt-get clean;apt-get autoclean)
    (apt-get dist-upgrade -y;apt-get autoremove --purge -y;apt-get clean;apt-get autoclean;hash -r)
    apt-get -qq install -y ssh
    dpkg --list | grep '^rc ' | awk '{ print $2 }' | xargs dpkg -P 2>/dev/null
    if [[ $platform = "Raspbian" ]]; then
        (apt-get -qq install --reinstall wpasupplicant -y;apt-get -qq clean;dpkg-reconfigure wpasupplicant)
    fi
    sed -i '/PermitRootLogin/c\PermitRootLogin yes' /etc/ssh/sshd_config
    cd /root
    mv .bashrc .bashrc.orig
    cat .bashrc.orig > .bashrc
    if [[ $platform = "Raspbian" ]]; then
        echo "/usr/src/utils/AllStar-build/rpi/update-stage2.sh" >> .bashrc
    else
        echo "/usr/src/utils/AllStar-build/debian/update-stage2.sh" >> .bashrc
    fi
    sync
    echo "Rebooting to finish install"
    echo "When your node reboots, you need to log in
to finish the upgrade."
    exit
    sudo reboot
}
# Run the upgrade
  distro
  check
  prompt
  update
exit 0
