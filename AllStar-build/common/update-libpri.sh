#! /bin/bash
# update-libpri.sh - Patch and build Libpri for All Star

# Script Start
echo "Building libpri..."
cd /usr/src/utils/astsrc/libpri/
# Patch libpri for use with AllStar
patch </usr/src/utils/AllStar-build/patches/patch-libpri-makefile
# Build and install libpri
(make all;make install)
echo "Done"
exit 0
