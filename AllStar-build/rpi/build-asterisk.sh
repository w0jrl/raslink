#! /bin/bash
# Used to patch and build asterisk for AllStar
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi's!
# You have been warned!

# Script Start
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
make config
make samples
# Build URI diag
cd ../uridiag
make install
# Clean out and replace samples
cd /etc/asterisk/
rm -rf *
cp /usr/src/utils/AllStar-build/configs/* .
# Install Nodelist update and startup scripts
cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist
chmod +x /usr/local/bin/rc.updatenodelist
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/asterisk.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/dahdi.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/updatenodelist.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/dahdi /etc/init.d
update-rc.d dahdi defaults
systemctl daemon-reload
systemctl enable asterisk.timer
systemctl enable dahdi.timer
systemctl enable updatenodelist.service
# add the sound files for app_rpt
cd /usr/src/utils/astsrc
cp -a /usr/src/utils/astsrc/sounds/* /var/lib/asterisk/sounds
echo "snd_pcm_oss" >> /etc/modules
systemctl start asterisk.service
echo "Done."

