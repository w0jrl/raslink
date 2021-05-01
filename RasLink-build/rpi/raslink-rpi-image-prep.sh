#!/bin/bash
# raslink-rpi-image-prep.sh: Prepare a clean image for RasLink on Raspbian
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
# For developers only!
# Do not use unless you know what you're doing,
# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pis!
# You have been warned!
#
# Script Start
# Rotate logs weekly
chmod +x /usr/src/utils/RasLink-build/common/mk-logrotate-asterisk.sh
/usr/src/utils/RasLink-build/common/mk-logrotate-asterisk.sh
# Make sure we are running the latest and greatest
(apt-get update;apt-get dist-upgrade -y)
# Install kernel and matching headers
apt-get install raspberrypi-kernel raspberrypi-kernel-headers
# Replace /boot/config.txt
cp /usr/src/utils/RasLink-build/rpi/boot-config.txt /boot/config.txt
# Setup Pulseaudio configuration
chmod +x /usr/src/utils/RasLink-build/common/update-audio-config.sh
/usr/src/utils/RasLink-build/common/update-audio-config.sh
# Configure packages
chmod +x /usr/src/utils/RasLink-build/rpi/chk-packages.sh
/usr/src/utils/RasLink-build/rpi/chk-packages.sh
# Setup for AllStar install
chmod +x /usr/src/utils/RasLink-build/rpi/raslink-rpi-install.sh
cpu=$(grep -ic 'for cpu' /etc/rc.local)
governor=$(grep -ic 'scaling_governor' /etc/rc.local)
if (( "$governor" >= "1" )) && [ "$cpu" = "0" ]; then
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
sync
echo "REBOOT before you run the install script"
exit 0
