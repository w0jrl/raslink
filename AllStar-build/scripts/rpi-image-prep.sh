#!/bin/sh
# Used to prepare a clean image for AllStar
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi 2s!
# You have been warned!

# Script Start
# change governor to performance
echo "performance" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# rotate logs on boot
# helps with write cycles of SD card
echo "tmpfs /var/log tmpfs defaults,noatime,nosuid,mode=0755,size=100m 0 0" >> /etc/fstab
# Make sure we are running the latest and greatest
(apt-get update;apt-get dist-upgrade -y)
# Install kernel and matching headers
apt-get install linux-image-rpi2-rpfv linux-headers-rpi2-rpfv		
# Replace /boot/config.txt
cp /usr/src/utils/AllStar-build/system/boot-config.txt /boot/config.txt
# install required packages
chmod +x /usr/src/utils/AllStar-build/scripts/required-tools.sh
/usr/src/utils/AllStar-build/scripts/required-tools.sh
# remove unneededpackages and sources
/usr/src/utils/AllStar-build/scripts/chk-packages.sh
sync
echo "REBOOT before you run the install script"

