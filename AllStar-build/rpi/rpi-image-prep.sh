#!/bin/bash
# Prepare a clean image for AllStar on Raspbian
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pis!
# You have been warned!

# Script Start
# rotate logs monthly
chmod +x /usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
/usr/src/utils/AllStar-build/common/mk-logrotate-asterisk.sh
# Make sure we are running the latest and greatest
(apt-get update;apt-get dist-upgrade -y)
# Install kernel and matching headers
apt-get install raspberrypi-kernel raspberrypi-kernel-headers
# Replace /boot/config.txt
cp /usr/src/utils/AllStar-build/rpi/boot-config.txt /boot/config.txt
# install required packages
chmod +x /usr/src/utils/AllStar-build/common/required-tools.sh
/usr/src/utils/AllStar-build/common/required-tools.sh
# remove unneededpackages and sources
chmod +x /usr/src/utils/AllStar-build/rpi/chk-packages.sh
/usr/src/utils/AllStar-build/rpi/chk-packages.sh
# set up for AllStar install
chmod +x /usr/src/utils/AllStar-build/rpi/rpi-allstar-install.sh
sync
echo "REBOOT before you run the install script"
exit 0
