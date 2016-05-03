#!/bin/bash
echo "Running install, stage two."
sleep 1
echo "Building Dahdi..."
sleep 1
cd /usr/src/utils/astsrc
cd ./dahdi*
make
make install
make config
echo "Done"
sleep 1
echo "Building libpri..."
sleep 1
cd ../libpri
make
make instal
sleep 1
systemctl daemon-reload
systemctl start dahdi
mv /etc/dahdi/modules /etc/dahdi/modules.old
echo "Done"
sleep 1
echo "Building asterisk..."
sleep 1
cd ../asterisk
./configure
make clean
make menuselect.makeopts
make
make install
make config
make samples
echo "Done"
sleep 1
echo "Building URI diag..."
cd ../uridiag
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
cp /usr/src/utils/AllStar-build/common/rc.local /etc/rc.local
(cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system/asterisk.service;systemctl daemon-reload)
(cp /usr/src/utils/AllStar-build/common/rc.allstar /usr/local/bin/rc.allstar;chmod +x /usr/local/bin/rc.allstar)
(cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
systemctl disable dahdi
systemctl disable asterisk
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
/usr/local/bin/rc.updatenodelist &
sleep 3
ln -fs /tmp/rpt_extnodes /var/lib/asterisk/rpt_extnodes
echo "Done"
sleep 1
# Restore bashrc
mv /root/.bashrc.orig /root/.bashrc
echo "AllStar is now installed..."
echo "You can update the system at any time by running 'system-update' at a root prompt..."
echo "Please edit rpt.conf, iax.conf, extensions.conf, and usbradio.conf."
echo "If you want to use EchoLink, edit echolink.conf."
echo "If you don't want to use EchoLink, you don't need to do anything."
echo "EchoLink is disabled by default."
echo "All files are located in /etc/asterisk."
echo "After editing files, reboot to get your node online."
echo "Enjoy AllStar on Debian!"

