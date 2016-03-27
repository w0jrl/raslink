#!/bin/sh
# Creates the system-update and node list links.

# Script Start.
ln -fs /usr/src/utils/AllStar-build/scripts/update-stage1.sh /usr/bin/system-update
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes

