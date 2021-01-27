#!/bin/bash
# update-stage2.sh - Update the system
# Stage Two
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
clear
# Make sure system-update runs in screen
if [ -z "$STY" ]; then exec screen -S system-update /bin/bash "$0"; fi
status() {
    "$@"
    if [ $? -ne 0 ]; then
        echo "PRESS ENTER TO EXIT"
        read
        exit 1
    fi
}
echo "RUNNING UPDATE; STAGE TWO"
echo -e "This will take a while.\nSystem-update is running in a screen session.\nIf your session disconnects during the update,\nafter reconnecting, run\n'screen -dr'\nto reconnect to\nthe update screen.\n"
systemctl stop asterisk.service osspd.service pulseaudio.service &>/dev/null &
echo -e "YOU CANNOT USE YOUR NODE DURING THIS PROCESS.\nIt has been disabled.\nPRESS ENTER TO CONTINUE"
read
# Restore bashrc
mv /root/.bashrc.orig /root/.bashrc
# Check for release upgrade
chmod +x /usr/src/utils/RasLink-build/common/release-upgrade.sh
status /usr/src/utils/RasLink-build/common/release-upgrade.sh
# Check and update repository URL
chmod +x /usr/src/utils/RasLink-build/common/remote-fetch.sh
status /usr/src/utils/RasLink-build/common/remote-fetch.sh
# Update repository branch name
chmod +x /usr/src/utils/RasLink-build/common/master-to-main.sh
status /usr/src/utils/RasLink-build/common/master-to-main.sh
# Make sure version runs at login
if [[ "$(grep -ic "/usr/bin/version" /root/.bashrc)" = "1" ]]; then
  sed -i '/\/usr\/bin\/version/d' /root/.bashrc
fi
if [[ "$(grep -ic "/usr/bin/version" /root/.profile)" = "0" ]]; then
  echo "/usr/bin/version" >> /root/.profile
fi
chmod +x /usr/src/utils/RasLink-build/rpi/chk-packages.sh
status /usr/src/utils/RasLink-build/rpi/chk-packages.sh
sleep 0.5s
chmod +x /usr/src/utils/RasLink-build/common/update-audio-config.sh
status /usr/src/utils/RasLink-build/common/update-audio-config.sh
sleep 0.5s
chmod +x /usr/src/utils/RasLink-build/common/update-dahdi.sh
status /usr/src/utils/RasLink-build/common/update-dahdi.sh
sleep 0.5s
chmod +x /usr/src/utils/RasLink-build/common/update-asterisk.sh
status /usr/src/utils/RasLink-build/common/update-asterisk.sh
sleep 0.5s
chmod +x /usr/src/utils/RasLink-build/common/update-uridiag.sh
status /usr/src/utils/RasLink-build/common/update-uridiag.sh
sleep 0.5s
chmod +x /usr/src/utils/RasLink-build/common/update-fail2ban.sh
status /usr/src/utils/RasLink-build/common/update-fail2ban.sh
sleep 0.5s
# Make sure configuration files and scripts are loaded
echo "Updating start up scripts..."
(cp /usr/src/utils/RasLink-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
(cp /usr/src/utils/RasLink-build/common/rc.nodenames /usr/local/bin/rc.nodenames;chmod +x /usr/local/bin/rc.nodenames)
(cp /usr/src/utils/RasLink-build/rpi/tmpfs.sh /usr/local/bin/tmpfs.sh;chmod +x /usr/local/bin/tmpfs.sh)
(cp /usr/src/utils/RasLink-build/rpi/zram.start /usr/src/utils/RasLink-build/rpi/zram.stop /usr/local/bin/.;chmod +x /usr/local/bin/zram.st*)
(cp /usr/src/utils/RasLink-build/common/timesync.hourly /usr/local/bin/timesync.hourly;chmod +x /usr/local/bin/timesync.hourly)
chmod +x /usr/src/utils/RasLink-build/rpi/make-links.sh
status /usr/src/utils/RasLink-build/rpi/make-links.sh
cp -a /usr/src/utils/src/sounds/* /var/lib/asterisk/sounds
gsmcount=$(find /var/lib/asterisk/sounds/rpt/ -maxdepth 1 -type f -name '*.gsm' -printf x | wc -c)
if [ "$gsmcount" -ne "0" ]; then
  rm -f /var/lib/asterisk/sounds/rpt/*.gsm
fi
if [ -e /usr/local/bin/zram.sh ]; then
  rm -f /usr/local/bin/zram.sh
fi
echo -e "Done\n"
sleep 0.5s
echo "Cleaning up object files..."
cd /usr/src/utils
(git checkout -f;git clean -f;rm -f 1) &>/dev/null
echo -e "Done\n"
sleep 0.5s
echo "Updating system boot configuration..."
cp /usr/src/utils/RasLink-build/rpi/boot-config.txt /boot/config.txt
cp /usr/src/utils/RasLink-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/common/updatenodelist.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/common/nodenames.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/rpi/tmpfs.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/rpi/zram.service /etc/systemd/system
cp /usr/src/utils/RasLink-build/common/timesync.service /etc/systemd/system
systemctl daemon-reload
systemctl enable pulseaudio.service osspd.service asterisk.service updatenodelist.service timesync.service &>/dev/null
systemctl enable avahi-daemon &>/dev/null
systemctl enable tmpfs.service &>/dev/null
systemctl enable zram.service &>/dev/null
if [ ! -e /root/.nonames ]; then
  systemctl enable nodenames.service &>/dev/null
fi
if [ -e /etc/systemd/system/fail2ban.service ]; then
  systemctl enable fail2ban.service &>/dev/null
fi
sed -i '/snd_bcm2835/d;/snd_pcm_oss/d;/snd_mixer_oss/d' /etc/modules
cpu=$(grep -ic 'for cpu' /etc/rc.local)
governor=$(grep -ic 'scaling_governor' /etc/rc.local)
if (( "$governor" >= "0" )) && [ "$cpu" = "0" ]; then
  sed -i '/scaling_governor/c\
for cpu in \/sys\/devices\/system\/cpu\/cpu\[0-9\]\*\; do echo -n performance \\\
| tee $cpu\/cpufreq\/scaling_governor\; done \&\>\/dev\/null' /etc/rc.local
else
  if [ "$governor" = "0" ] && [ "$cpu" = "0" ]; then
    sed -i '/Print the IP address/i\
# Set CPU governor to performance\
for cpu in \/sys\/devices\/system\/cpu\/cpu\[0-9\]\*\; do echo -n performance \\\
| tee $cpu\/cpufreq\/scaling_governor\; done \&\>\/dev\/null' /etc/rc.local
  fi
fi
$(which timedatectl) set-ntp off
(cp /usr/src/utils/RasLink-build/common/irqbalance.daily /etc/cron.daily/irqbalance;chmod +x /etc/cron.daily/irqbalance)
echo -e "Done\n"
sleep 0.5s
chmod +x /usr/src/utils/RasLink-build/common/system-update-messages.sh
/usr/src/utils/RasLink-build/common/system-update-messages.sh
echo -e "UPDATE COMPLETE\nYou can run this tool at any time by typing 'system-update' at a root prompt."
date +'%A, %B %d, %Y%t%t%T %Z' > /root/.lastupdate
echo "REBOOTING TO APPLY CHANGES"
sync
reboot
exit 0
