#! /bin/bash
# Used to build Dahdi for AllStar
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi's!
# You have been warned!

# Script Start
cd /usr/src/utils/astsrc/dahdi-linux-complete/
# Patch dahdi for use with AllStar Asterisk
# https://allstarlink.org/dude-dahdi-2.10.0.1-patches-20150306
# Soon to be included in the official release of DAHDI from Digium.
patch -p1 < /usr/src/utils/AllStar-build/patches/patch-dahdi-dude-current
# remove setting the owner to asterisk
patch -p0 < /usr/src/utils/AllStar-build/patches/patch-dahdi.rules
# Build and install dahdi
make all
make install
make config
systemctl daemon-reload
systemctl start dahdi

