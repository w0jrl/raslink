#!/bin/bash
# rpi-image-prep.sh - Prepare a clean image for AllStar on Raspbian
#    Copyright (C) 2017  Jeremy Lincicome (W0JRL)
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

# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pis!
# You have been warned!

# Script Start
# Rotate logs monthly
chmod +x /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
/usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
# Make sure we are running the latest and greatest
(apt-get update;apt-get dist-upgrade -y)
# Install kernel and matching headers
apt-get install raspberrypi-kernel raspberrypi-kernel-headers
# Replace /boot/config.txt
cp /usr/src/utils/AllStar-build/rpi/boot-config.txt /boot/config.txt
# Setup Alsa configuration
cp /usr/src/utils/AllStar-build/rpi/etc-asound.conf /etc/asound.conf
# Install required packages
chmod +x /usr/src/utils/AllStar-build/common/required-tools.sh
/usr/src/utils/AllStar-build/common/required-tools.sh
# Remove unneededpackages and sources
chmod +x /usr/src/utils/AllStar-build/rpi/chk-packages.sh
/usr/src/utils/AllStar-build/rpi/chk-packages.sh
# Setup for AllStar install
chmod +x /usr/src/utils/AllStar-build/rpi/rpi-allstar-install.sh
if [ "$(grep -ic "scaling_governor" /etc/rc.local)" == "0" ]; then
  sed -i '/Print the IP address/i\
# Set CPU governor to performance\
echo "performance" > \/sys\/devices\/system\/cpu\/cpu0\/cpufreq\/scaling_governor' /etc/rc.local
fi
sync
echo "REBOOT before you run the install script"
exit 0
