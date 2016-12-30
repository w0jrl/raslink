#! /bin/bash
# Used to patch and build asterisk for AllStar

# Script Start
echo "Building asterisk..."
cd /usr/src/utils
# put git commit number where asterisk makefile expects it
git log -1 --format="%h" > /usr/src/utils/astsrc/asterisk/.version
cd /usr/src/utils/astsrc/asterisk/
# patch for ulaw Core and Extras Sound Packages
patch < /usr/src/utils/AllStar-build/patches/patch-asterisk-menuselect.makeopts
# patch for SSL used in res_crypto
patch < /usr/src/utils/AllStar-build/patches/patch-configure
patch < /usr/src/utils/AllStar-build/patches/patch-configure.ac
# patch for LSB used in Debian init scripts
patch -p1 < /usr/src/utils/AllStar-build/patches/patch-rc-debian
patch < /usr/src/utils/AllStar-build/patches/patch-asterisk-makefile
# add the notch option
cp /usr/src/utils/astsrc/extras/notch/rpt_notch.c ./apps
sed -i 's/\/\* #include "rpt_notch.c" \*\//#include "rpt_notch.c"/' ./apps/app_rpt.c
# add mdc1200 support
cp /usr/src/utils/astsrc/extras/mdc1200/*.c ./apps
cp /usr/src/utils/astsrc/extras/mdc1200/*.h ./apps
sed -i 's/\/\* #include "mdc_decode.c" \*\//#include "mdc_decode.c"/' ./apps/app_rpt.c
sed -i 's/\/\* #include "mdc_encode.c" \*\//#include "mdc_encode.c"/' ./apps/app_rpt.c
# change TX enabled message
sed -i 's/"RPTENA"/"TXENA"/' ./apps/app_rpt.c
# configure the build process
./configure
# Build and install Asterisk
make
make install
echo "Done."
exit 0
