#!/bin/sh
# Used to install AllStar Raspbian on a clean image
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi 2s!
# You have been warned!

# Script Start
# change governor to performance
echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling-governor

chmod +x /usr/src/utils/AllStar-build/scripts/rebuild.sh
/usr/src/utils/AllStar-build/scripts/rebuild.sh

