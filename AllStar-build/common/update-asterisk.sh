#! /bin/bash
# Used to build asterisk for AllStar

# Script Start
echo "Building asterisk..."
cd /usr/src/utils/astsrc/asterisk/
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
# prevent reading false DTMF digits
sed -i 's/26.0 : 42.0/32.0 : 42.0/' ./main/dsp.c
# put git commit number where asterisk makefile expects it
git describe --always > .version
# configure the build process
./configure
# Build and install Asterisk
make; make install
echo "Done."
exit 0
