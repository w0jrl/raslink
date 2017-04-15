#! /bin/bash
# Used to build asterisk for AllStar

# Script Start
echo "Building asterisk..."
cd /usr/src/utils/astsrc/asterisk/
# put git commit number where asterisk makefile expects it
git describe --always > .version
# configure the build process
./configure
# Build and install Asterisk
(make all;make install)
echo "Done."
exit 0
