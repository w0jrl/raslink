#!/bin/bash
echo "Running Debian AllStar install, stage two."
sleep 1
cd /usr/src/utils
echo "Downloading and unpacking dahdi..."
wget http://downloads.asterisk.org/pub/telephony/dahdi-linux-complete/dahdi-linux-complete-current.tar.gz &>/dev/null
cd /usr/src/utils/astsrc
tar zxvf /usr/src/utils/dahdi-linux-complete-current.tar.gz &>/dev/null
mv dahdi* dahdi
rm -rf /usr/src/utils/*.tar.gz
echo "Done"
sleep 1
echo "Building Dahdi..."
cd /usr/src/utils/astsrc/dahdi
patch -p1 < /usr/src/utils/AllStar-build/patches/patch-dahdi-dude-current
# remove setting the owner to asterisk
patch -p0 < /usr/src/utils/AllStar-build/patches/patch-dahdi.rules
# Build and install dahdi
make
make install
make config
echo "Done"
sleep 1
echo "Building libpri..."
cd /usr/src/utils/astsrc/libpri
patch </usr/src/utils/AllStar-build/patches/patch-libpri-makefile
make
make instal
cp /usr/src/utils/AllStar-build/common/dahdi /etc/init.d
update-rc.d dahdi defaults
systemctl daemon-reload
systemctl start dahdi
echo "Done"
sleep 1
echo "Building asterisk..."
cd /usr/src/utils/astsrc/asterisk
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
# build and install asterisk
./configure
make
make install
make samples
echo "Done"
sleep 1
echo "Building URI diag..."
cd /usr/src/utils/astsrc/uridiag
make
make install
echo "Done"
sleep 1
echo "Setting up defaults for AllStar..."
cd /etc/asterisk/
rm -rf *
cp /usr/src/utils/AllStar-build/configs/* .
echo "Done"
sleep 1
echo "Installing default sound files..."
cp -a /usr/src/utils/astsrc/sounds/* /var/lib/asterisk/sounds
echo "snd_pcm_oss" >> /etc/modules
echo "Done"
sleep 1
echo "Setting up startup scripts..."
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/asterisk.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/dahdi.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/updatenodelist.service /etc/systemd/system
systemctl daemon-reload
systemctl enable asterisk.timer
systemctl enable dahdi.timer
systemctl enable updatenodelist.service
chmod +x /usr/src/utils/AllStar-build/common/asterisk-restart.sh
ln -fs /usr/src/utils/AllStar-build/common/asterisk-restart.sh /usr/bin/asterisk-restart
chmod +x /usr/src/utils/AllStar-build/common/uricheck.sh
ln -fs /usr/src/utils/AllStar-build/common/uricheck.sh /usr/bin/uricheck
echo "Done"
echo "Starting Asterisk..."
systemctl start asterisk
echo "Done"
sleep 1
echo "Setting up system update..."
chmod +x /usr/src/utils/AllStar-build/debian/update-stage1.sh
ln -fs /usr/src/utils/AllStar-build/debian/update-stage1.sh /usr/bin/system-update
chmod +x /usr/src/utils/AllStar-build/debian/update-stage2.sh
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
echo "Done"
sleep 1
echo "AllStar is now installed..."
echo "You can update the system at any time by running 'system-update' at a root prompt..."
echo "Please edit rpt.conf, iax.conf, extensions.conf, and usbradio.conf."
echo "If you want to use EchoLink, edit echolink.conf."
echo "If you don't want to use EchoLink, you don't need to do anything."
echo "EchoLink is disabled by default."
echo "All files are located in /etc/asterisk."
echo "After editing files, reboot to get your node online."
echo "Enjoy AllStar on Debian!"
# Restore bashrc
mv /root/.bashrc.orig /root/.bashrc
exit 0
