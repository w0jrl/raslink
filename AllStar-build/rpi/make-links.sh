#!/bin/sh
# Creates the system-update, wifi-setup, asterisk-restart, and node list links.

# Script Start.
ln -fs /usr/src/utils/AllStar-build/rpi/update-stage1.sh /usr/bin/system-update
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
ln -fs /usr/src/utils/AllStar-build/rpi/wifi-setup.sh /usr/bin/wifi-setup
chmod +x /usr/src/utils/AllStar-build/rpi/wifi-setup.sh
ln -fs /usr/src/utils/AllStar-build/common/asterisk-restart.sh /usr/bin/asterisk-restart
chmod +x /usr/src/utils/AllStar-build/common/asterisk-restart.sh

