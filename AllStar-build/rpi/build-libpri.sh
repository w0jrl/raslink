#! /bin/bash
# Patch and build Libpri for All Star
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi's!
# You have been warned!

# Script Start
cd /usr/src/utils/astsrc/libpri/
patch </usr/src/utils/AllStar-build/patches/patch-libpri-makefile
make
make install
cp /usr/src/utils/AllStar-build/common/dahdi /etc/init.d
update-rc.d dahdi defaults
systemctl daemon-reload
systemctl start dahdi

