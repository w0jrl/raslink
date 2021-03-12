#!/bin/bash
# raslink-rpi-install.sh - Automated build and patch script for AllStar Raspbian install
#    Copyright (C) 2021  Jeremy Lincicome (W0JRL)
#    https://jlappliedtechnologies.com  admin@jlappliedtechnologies.com
#
#    This program is free software; you can redistribute it and/or modify
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
#Used to update and prepare images for shipping
# Also used for resetting AllStar to defaults in test environments
# For developers only!
# Do not use unless you know what you're doing,
# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pis!
# You have been warned!
#
# Script Start
status() {
    $@
    if [ $? -ne 0 ]; then
        exit 1
    fi
}
echo "Setting up defaults for AllStar..."
mkdir -p /etc/asterisk
cd /etc/asterisk
cp /usr/src/utils/RasLink-build/configs/* .
echo "Done"
chmod +x /usr/src/utils/RasLink-build/common/update-dahdi.sh
status /usr/src/utils/RasLink-build/common/update-dahdi.sh
chmod +x /usr/src/utils/RasLink-build/common/update-asterisk.sh
status /usr/src/utils/RasLink-build/common/update-asterisk.sh
chmod +x /usr/src/utils/RasLink-build/common/update-uridiag.sh
status /usr/src/utils/RasLink-build/common/update-uridiag.sh
chmod +x /usr/src/utils/RasLink-build/common/update-fail2ban.sh
status /usr/src/utils/RasLink-build/common/update-fail2ban.sh
echo "Done"
echo "Installing default sound files..."
cp -a /usr/src/utils/src/sounds/* /var/lib/asterisk/sounds
echo "Done"
echo "Setting up startup scripts and system-update..."
(cp /usr/src/utils/RasLink-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
(cp /usr/src/utils/RasLink-build/common/rc.nodenames /usr/local/bin/rc.nodenames;chmod +x /usr/local/bin/rc.nodenames)
(cp /usr/src/utils/RasLink-build/rpi/tmpfs.sh /usr/local/bin/tmpfs.sh;chmod +x /usr/local/bin/tmpfs.sh)
(cp /usr/src/utils/RasLink-build/rpi/zram.sh /usr/local/bin/zram.sh;chmod +x /usr/local/bin/zram.sh)
(cp /usr/src/utils/RasLink-build/common/timesync.hourly /usr/local/bin/timesync.hourly;chmod +x /usr/local/bin/timesync.hourly)
(cp /usr/src/utils/RasLink-build/common/timesync.startup /usr/local/bin/timesync.startup;chmod +x /usr/local/bin/timesync.startup)
cp /usr/src/utils/RasLink-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/common/updatenodelist.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/common/nodenames.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/rpi/tmpfs.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/rpi/zram.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/common/timesync.service /etc/systemd/system
systemctl daemon-reload
systemctl enable pulseaudio.service osspd.service asterisk.service updatenodelist.service nodenames.service fail2ban.service timesync.service &>/dev/null
systemctl enable tmpfs.service &>/dev/null
systemctl enable zram.service &>/dev/null
$(which timedatectl) set-ntp off
(cp /usr/src/utils/RasLink-build/common/irqbalance.daily /etc/cron.daily/irqbalance;chmod +x /etc/cron.daily/irqbalance)
chmod +x /usr/src/utils/RasLink-build/rpi/make-links.sh
status /usr/src/utils/RasLink-build/rpi/make-links.sh
if [[ "$(grep -ic "/usr/bin/version" /root/.profile)" = "0" ]]; then
  echo "/usr/bin/version" >> /root/.profile
fi
echo "Done"
echo "Cleaning up object files..."
cd /usr/src/utils/
(git checkout -f;git clean -f;rm -f 1) &>/dev/null
echo "Done"
echo "AllStar is now installed."
date +'%A, %B %d, %Y%t%t%T %Z' > /root/.lastupdate
exit 0
