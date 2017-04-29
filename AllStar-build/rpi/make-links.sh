#!/bin/bash
# make-links.sh - Create the system-update, wifi-setup, asterisk-restart, uricheck, version, and node list links.

# Script Start
chmod +x /usr/src/utils/AllStar-build/rpi/update-stage1.sh
ln -fs /usr/src/utils/AllStar-build/rpi/update-stage1.sh /usr/bin/system-update
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
chmod +x /usr/src/utils/AllStar-build/rpi/wifi-setup.sh
ln -fs /usr/src/utils/AllStar-build/rpi/wifi-setup.sh /usr/bin/wifi-setup
chmod +x /usr/src/utils/AllStar-build/common/asterisk-restart.sh
ln -fs /usr/src/utils/AllStar-build/common/asterisk-restart.sh /usr/bin/asterisk-restart
chmod +x /usr/src/utils/AllStar-build/common/uricheck.sh
ln -fs /usr/src/utils/AllStar-build/common/uricheck.sh /usr/bin/uricheck
chmod +x /usr/src/utils/AllStar-build/common/version.sh
ln -fs /usr/src/utils/AllStar-build/common/version.sh /usr/bin/version
ln -fs /etc/network/if-up.d/ntpdate /etc/cron.hourly/ntpdate
exit 0
