#!/bin/bash
# update-stage2.sh - Update the system
# Stage Two
#    Copyright (C) 2019  Jeremy Lincicome (W0JRL)
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
    $@
    if [ $? -ne 0 ]; then
        sleep 5
        exit 1
    fi
}
echo "RUNNING UPDATE; STAGE TWO"
echo -e "This will take a while.\nSystem-update is running in a screen session.\nIf your session disconnects during the update,\nafter reconnecting, run\n'screen -dr'\nto reconnect to\nthe update screen.\n"
sleep 5
service asterisk stop &>/dev/null
echo -e "YOU CANNOT USE YOUR NODE DURING THIS PROCESS.\nIt has been disabled.\n"
sleep 1
# Restore bashrc
mv /root/.bashrc.orig /root/.bashrc
# Check for release upgrade
#chmod +x /usr/src/utils/AllStar-build/common/release-upgrade.sh
#status /usr/src/utils/AllStar-build/common/release-upgrade.sh
# Check and update repository URL
chmod +x /usr/src/utils/AllStar-build/common/remote-fetch.sh
/usr/src/utils/AllStar-build/common/remote-fetch.sh
# Make sure version runs at login
if [[ "$(grep -ic "/usr/bin/version" /root/.bashrc)" = "1" ]]; then
  sed -i '/\/usr\/bin\/version/d' /root/.bashrc
fi
if [[ "$(grep -ic "/usr/bin/version" /root/.profile)" = "0" ]]; then
  echo "/usr/bin/version" >> /root/.profile
fi
chmod +x /usr/src/utils/AllStar-build/rpi/chk-packages.sh
status /usr/src/utils/AllStar-build/rpi/chk-packages.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-dahdi.sh
status /usr/src/utils/AllStar-build/common/update-dahdi.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-asterisk.sh
status /usr/src/utils/AllStar-build/common/update-asterisk.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-uridiag.sh
status /usr/src/utils/AllStar-build/common/update-uridiag.sh
sleep 0.5
# Make sure configuration files and scripts are loaded
echo "Updating start up scripts..."
(cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
(cp /usr/src/utils/AllStar-build/common/rc.nodenames /usr/local/bin/rc.nodenames;chmod +x /usr/local/bin/rc.nodenames)
(cp /usr/src/utils/AllStar-build/rpi/tmpfs.sh /usr/local/bin/tmpfs.sh;chmod +x /usr/local/bin/tmpfs.sh)
(cp /usr/src/utils/AllStar-build/rpi/zram.sh /usr/local/bin/zram.sh;chmod +x /usr/local/bin/zram.sh)
(cp /usr/src/utils/AllStar-build/common/dsp.startup /usr/local/bin/dsp.startup;chmod +x /usr/local/bin/dsp.startup)
(cp /usr/src/utils/AllStar-build/common/timesync.hourly /usr/local/bin/timesync.hourly;chmod +x /usr/local/bin/timesync.hourly)
chmod +x /usr/src/utils/AllStar-build/rpi/make-links.sh
/usr/src/utils/AllStar-build/rpi/make-links.sh
cp -a /usr/src/utils/astsrc/allstar/sounds/* /var/lib/asterisk/sounds
gsmcount=`ls -1 /var/lib/asterisk/sounds/rpt/*.gsm 2>/dev/null | wc -l`
if [ "$gsmcount" != "0" ]; then
  rm -f /var/lib/asterisk/sounds/rpt/*.gsm
fi
echo -e "Done\n"
sleep 0.5
echo "Cleaning up object files..."
cd /usr/src/utils
git clean -f &>/dev/null
if [ -e /usr/src/utils/1 ]; then
    rm -f /usr/src/utils/1
fi
echo -e "Done\n"
sleep 0.5
echo "Updating system boot configuration..."
cp /usr/src/utils/AllStar-build/rpi/boot-config.txt /boot/config.txt
cp /usr/src/utils/AllStar-build/rpi/etc-asound.conf /etc/asound.conf
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/asterisk.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/updatenodelist.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/nodenames.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/timesync.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/rpi/tmpfs.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/rpi/zram.service /etc/systemd/system
systemctl daemon-reload
systemctl enable asterisk.timer &>/dev/null
systemctl enable updatenodelist.service &>/dev/null
systemctl enable avahi-daemon &>/dev/null
systemctl enable timesync.service &>/dev/null
systemctl enable tmpfs.service &>/dev/null
systemctl enable zram.service &>/dev/null
if [ ! -e /root/.nonames ]; then
  systemctl enable nodenames.service &>/dev/null
fi
if [[ "$(grep -ic "snd_bcm2835" /etc/modules)" = "1" ]]; then
  sed -i '/snd_bcm2835/d' /etc/modules
fi
if [ "$(grep -ic "snd_pcm_oss" /etc/modules)" == "0" ]; then
  echo "snd_pcm_oss" >> /etc/modules
fi
if [[ "$(grep -ic "snd_pcm_oss" /etc/modules)" > "1" ]]; then
  sed -i '/snd_pcm_oss/d' /etc/modules
  echo "snd_pcm_oss" >> /etc/modules
fi
if [ "$(grep -ic "snd_mixer_oss" /etc/modules)" == "0" ]; then
  echo "snd_mixer_oss" >> /etc/modules
fi
if [ "$(grep -ic "snd_mixer_oss" /etc/modules)" > "1" ]; then
  sed -i '/snd_mixer_oss/d' /etc/modules
  echo "snd_mixer_oss" >> /etc/modules
fi
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
echo -e "Done\n"
sleep 0.5
echo -e "UPDATE COMPLETE\nYou can run this tool at any time by typing 'system-update' at a root prompt."
date +'%A, %B %d, %Y%t%t%T %Z' > /root/.lastupdate
echo "REBOOTING TO APPLY CHANGES"
sync
reboot
exit 0
