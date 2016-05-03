#!/bin/sh -e
# Creates the system-update, wifi-setup, asterisk-restart, uricheck, and node list links.

# Script Start.
ln -fs /usr/src/utils/AllStar-build/rpi/update-stage1.sh /usr/bin/system-update
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
chmod +x /usr/src/utils/AllStar-build/rpi/wifi-setup.sh
ln -fs /usr/src/utils/AllStar-build/rpi/wifi-setup.sh /usr/bin/wifi-setup
chmod +x /usr/src/utils/AllStar-build/common/asterisk-restart.sh
ln -fs /usr/src/utils/AllStar-build/common/asterisk-restart.sh /usr/bin/asterisk-restart
chmod +x /usr/src/utils/AllStar-build/common/uricheck.sh
ln -fs /usr/src/utils/AllStar-build/common/uricheck.sh /usr/bin/uricheck

