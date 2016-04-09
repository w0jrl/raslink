#!/bin/sh
# Automated rebuild and patch script for AllStar Raspbian install
#Used to update and prepare images for shipping
# Also used for resetting AllStar to defaults in test environments
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi's!
# You have been warned!

# Script Start
chmod +x /usr/src/utils/AllStar-build/rpi/get-src.sh
/usr/src/utils/AllStar-build/rpi/get-src.sh
chmod +x /usr/src/utils/AllStar-build/rpi/build-dahdi.sh
/usr/src/utils/AllStar-build/rpi/build-dahdi.sh
chmod +x /usr/src/utils/AllStar-build/rpi/build-libpri.sh
/usr/src/utils/AllStar-build/rpi/build-libpri.sh
chmod +x /usr/src/utils/AllStar-build/rpi/build-asterisk.sh
/usr/src/utils/AllStar-build/rpi/build-asterisk.sh
sleep 2
chmod +x /usr/src/utils/AllStar-build/rpi/make-links.sh
/usr/src/utils/AllStar-build/rpi/make-links.sh
sync

