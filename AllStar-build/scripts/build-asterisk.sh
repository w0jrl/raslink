#! /bin/sh
# Used to patch and build asterisk for AllStar
# For developers only!
# Do not use unless you know what you're doing,
	# and you've made a complete backup first!
# This script can break your node!
# This script should only be run on development Raspberry Pi 2s!
# You have been warned!

# Script Start
cd /usr/src/utils/astsrc/asterisk/
# clean the code
./configure
make clean
# patch for ulaw Core and Extras Sound Packages
patch < /usr/src/utils/AllStar-build/patches/patch-asterisk-menuselect.makeopts
# patch for SSL used in res_crypto
patch < /usr/src/utils/AllStar-build/patches/patch-configure
patch < /usr/src/utils/AllStar-build/patches/patch-configure.ac
# patch for LSB used in Debian init scripts
patch -p1 < /usr/src/utils/AllStar-build/patches/patch-rc-debian
patch < /usr/src/utils/AllStar-build/patches/patch-asterisk-makefile
./configure
# Build and install Asterisk
make menuselect.makeopts
make all
make install
make config
make samples
cp /usr/src/utils/AllStar-build/system/asterisk.service /etc/systemd/system
systemctl daemon-reload
# Clean out and replace samples
cd /etc/asterisk/
rm -rf *
cp /usr/src/utils/AllStar-build/configs/* .
# Install Nodelist update and start at boot
cp /usr/src/utils/AllStar-build/system/rc.updatenodelist /usr/local/bin/rc.updatenodelist
chmod +x /usr/local/bin/rc.updatenodelist
cp /usr/src/utils/AllStar-build/system/rc.local /etc/rc.local
cp /usr/src/utils/AllStar-build/system/rc.allstar /usr/local/bin/rc.allstar
chmod +x /usr/local/bin/rc.allstar
# Remove dahdi and asterisk from runlevel scripts
# rc.allstar will load them
systemctl disable asterisk
systemctl disable dahdi
# add the sound files for app_rpt
cd /usr/src/utils/astsrc
cp -a /usr/src/utils/astsrc/sounds/* /var/lib/asterisk/sounds

# make /dev/dsp available
# not needed for a hub
# Though it will not hurt anything.
echo "snd_pcm_oss" >> /etc/modules
systemctl stop dahdi
sleep 1
/usr/local/bin/rc.allstar
echo "Done."

