#! /bin/sh -e
# Used to patch and build Libpri for All Star
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi's!
# You have been warned!

# Script Start
cd /usr/src/utils/astsrc/libpri/
make clean
# Patch libpri for use with AllStar Asterisk
patch </usr/src/utils/AllStar-build/patches/patch-libpri-makefile
make all
make install

