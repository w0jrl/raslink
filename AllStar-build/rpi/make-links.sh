#!/bin/sh
# Creates the system-update, wifi-setup, and node list links.

# Script Start.
ln -fs /usr/src/utils/AllStar-build/rpi/update-stage1.sh /usr/bin/system-update
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
ln -fs /usr/src/utils/AllStar-build/rpi/wifi-setup.sh /usr/bin/wifi-setup
