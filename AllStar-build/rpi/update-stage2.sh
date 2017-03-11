#!/bin/bash
# Used to update the system
# Stage Two

# Script Start
echo "Running update, stage two."
echo "This will take a while."
sleep 0.5
(killall rc.local;systemctl stop dahdi.timer;systemctl stop asterisk.timer;systemctl stop asterisk.service;systemctl stop dahdi) &>/dev/null
sleep 0.5
echo "Your node can not be used durring this process. It has been disabled."
chmod +x /usr/src/utils/AllStar-build/rpi/chk-packages.sh
/usr/src/utils/AllStar-build/rpi/chk-packages.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-dahdi.sh
/usr/src/utils/AllStar-build/common/update-dahdi.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-libpri.sh
/usr/src/utils/AllStar-build/common/update-libpri.sh
sleep 0.5
chmod +x /usr/src/utils/AllStar-build/common/update-asterisk.sh
/usr/src/utils/AllStar-build/common/update-asterisk.sh
sleep 0.5
echo "Building URI diag..."
cd /usr/src/utils/astsrc/uridiag
make; make install
echo "Done"
sleep 0.5
# make sure configuration files and scripts are loaded
echo "Updating start up scripts..."
(cp /usr/src/utils/AllStar-build/common/rc.updatenodelist /usr/local/bin/rc.updatenodelist;chmod +x /usr/local/bin/rc.updatenodelist)
chmod +x /usr/src/utils/AllStar-build/rpi/make-links.sh
/usr/src/utils/AllStar-build/rpi/make-links.sh
cp -a /usr/src/utils/astsrc/sounds/* /var/lib/asterisk/sounds
gsmcount=`ls -1 /var/lib/asterisk/sounds/rpt/*.gsm 2>/dev/null | wc -l`
if [ $gsmcount != 0 ]; then
  rm -f/var/lib/asterisk/sounds/rpt/*.gsm
fi
echo "Done"
sleep 0.5
echo "Resetting compiler flags..."
cd /usr/src/utils
git clean -f
git checkout -f
echo "Done"
sleep 0.5
echo "Updating system boot configuration..."
cp /usr/src/utils/AllStar-build/rpi/boot-config.txt /boot/config.txt
cp /usr/src/utils/AllStar-build/rpi/etc-asound.conf /etc/asound.conf
cp /usr/src/utils/AllStar-build/common/asterisk.service /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/asterisk.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/dahdi.timer /etc/systemd/system
cp /usr/src/utils/AllStar-build/common/updatenodelist.service /etc/systemd/system
systemctl daemon-reload
systemctl enable asterisk.timer
systemctl enable dahdi.timer
systemctl enable updatenodelist.service
if [ `grep -ic "snd_bcm2835" /etc/modules` -eq 1 ]; then
  sed -i '/snd_bcm2835/d' /etc/modules
fi
if [ `grep -ic "snd_pcm_oss" /etc/modules` -gt 1 ]; then
  sed -i '/snd_pcm_oss/d' /etc/modules
  echo "snd_pcm_oss" >> /etc/modules
fi
echo "Done"
sleep 0.5
# restore bashrc
mv /root/.bashrc.orig /root/.bashrc
echo "The update is complete."
echo "You can run this tool at any time by typing 'system-update' at a root prompt."
echo "Rebooting your node to apply the changes"
sync
sudo reboot
exit 0
