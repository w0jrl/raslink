#!/bin/bash
echo "Running Debian AllStar install, stage two."
chmod +x /usr/src/utils/AllStar-build/common/update-dahdi.sh
/usr/src/utils/AllStar-build/common/update-dahdi.sh
chmod +x /usr/src/utils/AllStar-build/common/update-libpri.sh
/usr/src/utils/AllStar-build/common/update-libpri.sh
chmod +x /usr/src/utils/AllStar-build/common/update-asterisk.sh
/usr/src/utils/AllStar-build/common/update-asterisk.sh
echo "Building URI diag..."
cd /usr/src/utils/astsrc/uridiag
make; make install
echo "Done"
echo "Setting up defaults for AllStar..."
mkdir /etc/asterisk
cd /etc/asterisk
cp /usr/src/utils/AllStar-build/configs/* .
echo "Done"
echo "Installing default sound files..."
cp -a /usr/src/utils/astsrc/sounds/* /var/lib/asterisk/sounds
if [ `grep -ic "snd_bcm2835" /etc/modules` -eq 1 ]; then
  sed -i '/snd_bcm2835/d' /etc/modules
fi
if [ `grep -ic "snd_pcm_oss" /etc/modules` -gt 1 ]; then
  sed -i '/snd_pcm_oss/d' /etc/modules
  echo "snd_pcm_oss" >> /etc/modules
fi
echo "Done"
echo "Setting up startup scripts and system-update..."
(cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/asterisk.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/dahdi.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/updatenodelist.service /etc/systemd/system
systemctl daemon-reload
systemctl enable asterisk.timer
systemctl enable dahdi.timer
systemctl enable updatenodelist.service
chmod +x /usr/src/utils/AllStar-build/debian/make-links.sh
/usr/src/utils/AllStar-build/debian/make-links.sh
service cron restart
if [ `grep -ic "/usr/bin/version" /root/.profile` -eq 0 ]; then
  echo "/usr/bin/version" >> /root/.profile
fi
echo "Done"
echo "Starting Asterisk..."
systemctl start asterisk
echo "Done"
echo "AllStar is now installed..."
echo "You can update the system at any time by running 'system-update' at a root prompt."
echo "Please edit rpt.conf, iax.conf, extensions.conf, and usbradio.conf."
echo "If you want to use EchoLink, edit echolink.conf."
echo "If you don't want to use EchoLink, you don't need to do anything."
echo "EchoLink is disabled by default."
echo "All files are located in /etc/asterisk."
echo "After editing files, reboot to get your node online."
echo "Enjoy AllStar on Debian!"
# Restore bashrc
mv /root/.bashrc.orig /root/.bashrc
if [ `grep -ic "/usr/bin/version" /root/.bashrc` -eq 0 ]; then
  echo "/usr/bin/version" >> /root/.bashrc
fi
date > /root/.lastupdate
exit 0
