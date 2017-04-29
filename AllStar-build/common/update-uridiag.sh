#!/bin/bash
# update-uridiag.sh - Used to build Uridiag
# Needed for Uricheck to work

# Script Start
echo "Building Uridiag..."
cd /usr/src/utils/astsrc/uridiag/
(make;make install)
echo "Done"
exit 0
